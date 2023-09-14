#include "mbox.h"
#include "gpio.h"
#include "uart.h"
#include "./gcclib/stdint.h"
#include "./gcclib/stdarg.h"
#define NULL ((void *)0)

/* Mailbox Data Buffer (each element is 32-bit)*/
/*
 * The keyword attribute allows you to specify special attributes
 *
 * The aligned(N) attribute aligns the current data item on an address
 * which is a multiple of N, by inserting padding bytes before the data item
 *
 * __attribute__((aligned(16)) : allocate the variable on a 16-byte boundary.
 *
 *
 * We must ensure that our our buffer is located at a 16 byte aligned address,
 * so only the high 28 bits contain the address
 * (last 4 bits is ZERO due to 16 byte alignment)
 *
 */

// Initialize 32-bit mailbox data buffer aligned to 16 bytes
volatile unsigned int __attribute__((aligned(16))) mbox[36];

// Function to set up a mailbox buffer
void mbox_buffer_setup(unsigned int buffer_addr, unsigned int tag_identifier,
                       unsigned int **res_data, unsigned int res_length,
                       unsigned int req_length, unsigned int *request_values)
{
    // Initialize the variable argument list
    va_list args;

    // Pointer to the buffer in volatile memory
    volatile unsigned int *buf = (volatile unsigned int *)buffer_addr;

    // Initialize the buffer header
    buf[0] = 3 * sizeof(unsigned int) + req_length + res_length; // buffer size
    buf[1] = MBOX_REQUEST;

    // Initialize the tag header
    buf[2] = tag_identifier;
    buf[3] = res_length;
    buf[4] = req_length;

    // Assign the buffer data location for response data
    *res_data = &buf[5];

    // Copy the request values to the buffer
    for (int i = 0; i < req_length / sizeof(unsigned int); i++)
    {
        buf[5 + i] = request_values[i];
    }

    // End the buffer with a tag end marker
    buf[5 + (req_length / sizeof(unsigned int))] = MBOX_TAG_LAST;
}

// Show board revision in a human-readable manner.
void show_board_revision()
{
    unsigned int *data = 0; // Pointer to data in mailbox buffer

    // Set up mailbox buffer for board revision inquiry.
    mbox_buffer_setup((unsigned int)mbox, 0x00010002, &data, 4, 0, NULL);

    // Make the mailbox call and display board revision if successful.
    if (mbox_call((unsigned int)mbox, MBOX_CH_PROP))
    {
        uart_puts("Board Revision: ");
        uart_hex(*data); // Data will contain the board revision.
        uart_puts("\n");
    }
    else
    {
        uart_puts("Failed to retrieve board revision.\n");
    }
}

// Show board MAC address in a human-readable format.
void show_board_mac_address()
{
    unsigned int *data = 0; // Pointer to data in mailbox buffer
    unsigned int buf[2];    // Store MAC address in two 32-bit integers

    // Set up mailbox buffer for MAC address inquiry.
    mbox_buffer_setup((unsigned int)mbox, 0x00010003, &data, 8, 0, NULL);

    // Make the mailbox call and display MAC address if successful.
    if (mbox_call((unsigned int)mbox, MBOX_CH_PROP))
    {
        uart_puts("MAC Address: ");
        buf[0] = data[0]; // First four bytes of MAC address
        buf[1] = data[1]; // Last two bytes of MAC address

        // Format and print the MAC address in a readable form.
        uart_hex_byte((buf[1] >> 8) & 0xFF);
        uart_puts(":");
        uart_hex_byte(buf[1] & 0xFF);
        uart_puts(":");
        uart_hex_byte((buf[0] >> 24) & 0xFF);
        uart_puts(":");
        uart_hex_byte((buf[0] >> 16) & 0xFF);
        uart_puts(":");
        uart_hex_byte((buf[0] >> 8) & 0xFF);
        uart_puts(":");
        uart_hex_byte(buf[0] & 0xFF);

        uart_puts("\n");
    }
    else
    {
        uart_puts("Failed to retrieve MAC address.\n");
    }
}

// Function to display ARM frequency
void show_arm_frequency()
{
    unsigned int request_values[] = {3, 0};
    unsigned int *data = 0; // Pointer to the data in the mailbox buffer

    // Setup the mailbox buffer with the appropriate tag for ARM CPU frequency
    mbox_buffer_setup((unsigned int)mbox, 0x00030002, &data, 4, 0, NULL);

    // Make the mailbox call
    if (mbox_call((unsigned int)mbox, MBOX_CH_PROP))
    {
        uart_puts("The ARM Frequency is: ");
        uart_dec(*(data + 1)); // Call your uart_dec function to display the frequency
        uart_puts(" Hz\n");
    }
    else
    {
        uart_puts("Unable to get ARM frequency\n");
    }
}

// Function to display UART frequency
void show_uart_frequency()
{
    unsigned int *data = 0;                 // Pointer to the data in the mailbox buffer
    unsigned int request_values[] = {2, 0}; // 2 is the clock id for UART

    // Setup the mailbox buffer with the appropriate tag for UART frequency
    mbox_buffer_setup((unsigned int)mbox, 0x00030002, &data, 8, 8, request_values);

    // Make the mailbox call
    if (mbox_call((unsigned int)mbox, MBOX_CH_PROP))
    {
        uart_puts("The UART Frequency is: ");
        uart_dec(*(data + 1)); // data+1 will point to the frequency value
        uart_puts(" Hz\n");
    }
    else
    {
        uart_puts("Unable to get UART frequency\n");
    }
}
/**
 * Read from the mailbox
 */
uint32_t mailbox_read(unsigned char channel)
{
    // Receiving message is buffer_addr & channel number
    uint32_t res;

    // Make sure that the message is from the right channel
    do
    {
        // Make sure there is mail to receive
        do
        {
            asm volatile("nop");
        } while (*MBOX0_STATUS & MBOX_EMPTY);

        // Get the message
        res = *MBOX0_READ;
    } while ((res & 0xF) != channel);

    return res;
}

/**
 * Write to the mailbox
 */
void mailbox_send(uint32_t msg, unsigned char channel)
{
    // Sending message is buffer_addr & channel number

    // Make sure you can send mail
    do
    {
        asm volatile("nop");
    } while (*MBOX1_STATUS & MBOX_FULL);

    // send the message
    *MBOX1_WRITE = msg;
}

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned int buffer_addr, unsigned char channel)
{
    // Check Buffer Address
    //	uart_puts("Buffer Address: ");
    //	uart_hex(buffer_addr);
    //	uart_sendc('\n');

    // Prepare Data (address of Message Buffer)
    unsigned int msg = (buffer_addr & ~0xF) | (channel & 0xF);
    mailbox_send(msg, channel);

    /* now wait for the response */
    /* is it a response to our message (same address)? */
    if (msg == mailbox_read(channel))
    {
        /* is it a valid successful response (Response Code) ? */
        //		if (mbox[1] == MBOX_RESPONSE)
        //			uart_puts("Got successful response \n");

        return (mbox[1] == MBOX_RESPONSE);
    }

    return 0;
}
