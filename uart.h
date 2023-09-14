#include "gpio.h"

/* Auxilary mini UART (UART1) registers */
#define AUX_ENABLE (*(volatile unsigned int *)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO (*(volatile unsigned int *)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER (*(volatile unsigned int *)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR (*(volatile unsigned int *)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR (*(volatile unsigned int *)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR (*(volatile unsigned int *)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR (*(volatile unsigned int *)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR (*(volatile unsigned int *)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH (*(volatile unsigned int *)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL (*(volatile unsigned int *)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT (*(volatile unsigned int *)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD (*(volatile unsigned int *)(MMIO_BASE + 0x00215068))

/* Function prototypes */
void uart_init();                                  // Initialize UART
void uart_sendc(char c);                           // Send a single character
char uart_getc();                                  // Receive a single character
void uart_puts(const char *s);                     // Send a string
void uart_gets(char *buffer, unsigned int length); // Receive a string up to a given length
char *strcpy(char *dest, const char *src);         // Copy string from 'src' to 'dest'
void uart_dec(int num);                            // Send decimal number as ASCII
void uart_hex(unsigned int d);                     // Send hexadecimal number as ASCII
void uart_hex_byte(unsigned int n);                // Send a single byte in hexadecimal format