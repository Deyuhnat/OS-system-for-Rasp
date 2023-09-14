#include "uart1.h"
#include "mbox.h"

// Macros
#define MAX_CMD_SIZE 100
#define MAX_HISTORY 10
#define NULL ((void *)0)

// Function prototypes
int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int n);
char *strtok(char *str, const char *delim);
int strspn(const char *str, const char *set);
int strcspn(const char *str, const char *set);
char *strchr(const char *str, int c);

// Global variables
char cmd_history[MAX_HISTORY][MAX_CMD_SIZE];
int history_index = 0;
int current_index = 0;

// Function to print welcome message at start
void print_welcome_message()
{
    // ASCII art and information displayed at startup
    uart_puts("d88888b d88888b d88888b d888888b .d888b.   j88D  .d888b.  .d88b. \n");
    uart_puts("88'     88'     88'     `~~88~~' VP  `8D  j8~88  88' `8D .8P  88.\n");
    uart_puts("88ooooo 88ooooo 88ooooo    88       odD' j8' 88  `V8o88' 88  d'88\n");
    uart_puts("88~~~~~ 88~~~~~ 88~~~~~    88     .88'   V88888D    d8'  88 d' 88\n");
    uart_puts("88.     88.     88.        88    j88.        88    d8'   `88  d8'\n");
    uart_puts("Y88888P Y88888P Y88888P    YP    888888D     VP   d8'     `Y88P' \n");
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("db   d8b   db d88888b db       .o88b.  .d88b.  .88b  d88. d88888b   d888888b  .d88b.  \n");
    uart_puts("88   I8I   88 88'     88      d8P  Y8 .8P  Y8. 88'YbdP`88 88'       `~~88~~' .8P  Y8. \n");
    uart_puts("88   I8I   88 88ooooo 88      8P      88    88 88  88  88 88ooooo      88    88    88 \n");
    uart_puts("Y8   I8I   88 88~~~~~ 88      8b      88    88 88  88  88 88~~~~~      88    88    88 \n");
    uart_puts("`8b d8'8b d8' 88.     88booo. Y8b  d8 `8b  d8' 88  88  88 88.          88    `8b  d8' \n");
    uart_puts(" `8b8' `8d8'  Y88888P Y88888P  `Y88P'  `Y88P'  YP  YP  YP Y88888P      YP     `Y88P' \n");
    uart_puts("\n");
    uart_puts("db   db db    db db    db    .d88b.  .d8888. \n");
    uart_puts("88   88 88    88 `8b  d8'   .8P  Y8. 88'  YP \n");
    uart_puts("88ooo88 88    88  `8bd8'    88    88 `8bo.  \n");
    uart_puts("88~~~88 88    88    88      88    88   `Y8b.\n");
    uart_puts("88   88 88b  d88    88      `8b  d8' db   8D \n");
    uart_puts("YP   YP ~Y8888P'    YP       `Y88P'  `8888Y' \n");
    uart_puts("\n");
    uart_puts("Developed By Nguyen Tan Huy - <s3864185>\n");
}

// Array of available commands
char *commands[] = {};
int num_commands = sizeof(commands) / sizeof(commands[0]);
// Color definitions
const char *colors[] = {
    "black",
    "red",
    "green",
    "yellow",
    "blue",
    "purple",
    "cyan",
    "white"};

const char *ansiColors[] = {
    "\033[30m",
    "\033[31m",
    "\033[32m",
    "\033[33m",
    "\033[34m",
    "\033[35m",
    "\033[36m",
    "\033[37m",
};

const char *ansiBackgroundColors[] = {
    "\033[40m",
    "\033[41m",
    "\033[42m",
    "\033[43m",
    "\033[44m",
    "\033[45m",
    "\033[46m",
    "\033[47m",
};
// Function to convert string to lowercase
void lowercaseLetter(char *str)
{
    // Convert each letter in str to lowercase
    while (*str)
    {
        if (*str >= 'A' && *str <= 'Z')
        {
            *str = *str + ('a' - 'A');
        }
        str++;
    }
}
// Function to set the text and background color
void setcolor(const char *textColor, const char *backgroundColor)
{
    char upperTextColor[MAX_CMD_SIZE] = {0};
    char upperBackgroundColor[MAX_CMD_SIZE] = {0};

    // Logic to set text and background color
    if (textColor)
    {
        strcpy(upperTextColor, textColor);
        lowercaseLetter(upperTextColor);
    }
    if (backgroundColor)
    {
        strcpy(upperBackgroundColor, backgroundColor);
        lowercaseLetter(upperBackgroundColor);
    }

    if (textColor)
    {
        for (int i = 0; i < 8; i++)
        {
            if (strcmp(upperTextColor, colors[i]) == 0)
            {
                uart_puts(ansiColors[i]);
            }
        }
    }
    if (backgroundColor)
    {
        for (int i = 0; i < 8; i++)
        {
            if (strcmp(upperBackgroundColor, colors[i]) == 0)
            {
                uart_puts(ansiBackgroundColors[i]);
            }
        }
    }
}

// Function to display the command prompt
void display_prompt()
{
    uart_puts("HuyOS> ");
}

// Function to clear the screen
void clear_screen()
{
    uart_puts("\033[H\033[J");
}
// Function to execute user commands
void execute_command(const char *cmd)
{ // Checks for each command and performs the action accordingly
    if (strcmp(cmd, "help") == 0)
    {
        // Display available commands and their usage
        uart_puts("Available commands:\n");
        uart_puts("  help         - Show brief information of all commands.\n");
        uart_puts("  clear        - Clear the screen.\n");
        uart_puts("  setcolor     - Set console text and/or background color.\n");
        uart_puts("  printf       - Run printf tests.\n");
        uart_puts("  extend       - Extend terminal height and width.\n");
        uart_puts("  showinfo     - Show board revision and MAC address.\n");
        uart_puts("  macinfo      - Show MAC address.\n");
        uart_puts("  armfreq      - Show ARM frequency.\n");
        uart_puts("  uartfreq     - Show UART frequency.\n");
        uart_puts("\nTo show full information of a specific command, type: help <command_name>\n");
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        clear_screen();
    }
    else if (strncmp(cmd, "setcolor", 8) == 0)
    {
        char *token = strtok(cmd, " ");
        char *textColor = NULL;
        char *backgroundColor = NULL;

        while (token != NULL)
        {
            if (strcmp(token, "-t") == 0)
            {
                token = strtok(NULL, " ");
                textColor = token;
            }
            else if (strcmp(token, "-b") == 0)
            {
                token = strtok(NULL, " ");
                backgroundColor = token;
            }
            token = strtok(NULL, " ");
        }

        setcolor(textColor, backgroundColor);
    }
    else if (strcmp(cmd, "help clear") == 0)
    {
        uart_puts("Clear screen - Scrolls down to the current cursor position in the terminal.\n");
    }
    else if (strcmp(cmd, "help setcolor") == 0)
    {
        uart_puts("Set console colors - Set text and/or background color of the console.\n"
                  "Available Colors: BLACK, RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE\n"
                  "Syntax: setcolor -t <text_color> -b <background_color>\n"
                  "Example: setcolor -t RED -b GREEN\n");
    }
    else if (strcmp(cmd, "help showinfo") == 0)
    {
        uart_puts("Show Information - Display board revision and MAC address in a meaningful format.\n");
    }
    else if (strcmp(cmd, "help macinfo") == 0)
    {
        uart_puts("MAC Information - Display the MAC address of the board.\n");
    }
    else if (strcmp(cmd, "help armfreq") == 0)
    {
        uart_puts("ARM Frequency - Display the frequency at which the ARM CPU is running.\n");
    }
    else if (strcmp(cmd, "help uartfreq") == 0)
    {
        uart_puts("UART Frequency - Display the frequency at which the UART is operating.\n");
    }
    else if (strcmp(cmd, "help printf") == 0)
    {
        uart_puts("Printf Command - Run printf tests to display formatted text.\n"
                  "Syntax: printf\n"
                  "Example: printf\n");
    }
    else if (strcmp(cmd, "printf") == 0)
    {
        // Various printf tests
        printf("String: %s\n", "Hello Boy!");
        printf("Character: %c\n", 69);
        printf("Percentage sign: %%\n");
        printf("Decimal/integer number: %d\n", 283);
        printf("This is a Float number: %f \n", (float)5 / 7);
        printf("Hexadecimal: %x\n", 161);
        uart_puts("Printf tests completed.\n");
    }
    else if (strcmp(cmd, "showinfo") == 0)
    {
        // Show board info
        show_board_revision();
        show_board_mac_address();
        uart_puts("Board information displayed successfully.\n");
    }
    else if (strcmp(cmd, "macinfo") == 0)
    {
        // Show MAC info
        show_board_mac_address();
        uart_puts("MAC address displayed successfully.\n");
    }
    else if (strcmp(cmd, "extend") == 0)
    {
        // Set terminal dimensions
        unsigned int request_vals[2] = {1024, 768};
        unsigned int *physize = 0; // Pointer to response data
        mbox_buffer_setup(ADDR(mbox), MBOX_TAG_SETPHYWH, &physize, 8, 8, request_vals);
        mbox_call(ADDR(mbox), MBOX_CH_PROP);
        uart_puts("Physical Width: ");
        uart_dec(physize[0]);
        uart_puts("\nPhysical Height: ");
        uart_dec(physize[1]);
        uart_puts("\nTerminal dimensions set.\n");
    }
    else if (strcmp(cmd, "armfreq") == 0)
    {
        // Show ARM frequency
        show_arm_frequency();
        uart_puts("ARM frequency displayed successfully.\n");
    }
    else if (strcmp(cmd, "uartfreq") == 0)
    {
        // Show UART frequency
        show_uart_frequency();
        uart_puts("UART frequency displayed successfully.\n");
    }
    else
    {
        uart_puts("Unknown command, please check again.\n");
    }
}

// Main function for CLI
void cli()
{
    static char cli_buffer[MAX_CMD_SIZE];
    static int index = 0;
    char c;
    while (1)
    { // Code to read user input and execute commands
        c = uart_getc();
        uart_sendc(c);

        // Implement tab completion
        if (c == '\t')
        {
            uart_puts("\nAuto-completion:  ");
            uart_puts(cli_buffer);
            // Iterate over the list of commands to find a match
            for (int i = 0; i < num_commands; i++)
            {
                // If the current user input matches the beginning of any command
                if (strncmp(cli_buffer, commands[i], strlen(cli_buffer)) == 0)
                {
                    // Autocomplete the command
                    strcpy(cli_buffer, commands[i]);
                    uart_puts(&cli_buffer[index]); // print remaining characters
                    index = strlen(cli_buffer);
                    break;
                }
            }
        }
        // Implement history scroll up
        else if (c == '_')
        {
            if (current_index == history_index)
            {
                // Navigate backwards in history
                // Store the current command if at the end of the history
                strcpy(cmd_history[history_index % MAX_HISTORY], cli_buffer);
            }
            current_index = (current_index - 1 + MAX_HISTORY) % MAX_HISTORY;
            strcpy(cli_buffer, cmd_history[current_index]);
            index = strlen(cli_buffer);
            // Refresh the display line
            uart_puts("\rHuyOS>                                                             ");
            uart_puts("\rHuyOS> ");
            uart_puts(cli_buffer);
        }
        // Implement history scroll down
        else if (c == '+')
        {
            // Navigate forwards in history
            current_index = (current_index + 1) % MAX_HISTORY;
            if (current_index == history_index)
            {
                strcpy(cli_buffer, cmd_history[current_index]);
            }
            else
            {
                strcpy(cli_buffer, cmd_history[current_index]);
            }
            index = strlen(cli_buffer);
            // Refresh the display line
            uart_puts("\rHuyOS>                                                               ");
            uart_puts("\rHuyOS> ");
            uart_puts(cli_buffer);
        }
        // Handle backspace
        else if (c == 0x08 || c == 0x7F)
        {
            // If there are characters to delete
            if (index > 0)
            {
                index--;
                cli_buffer[index] = '\0';
                // Remove the last character from the display
                uart_puts("\b \b");
                // Refresh the display line
                uart_puts("\rHuyOS>                                                                                                                                                    ");
                uart_puts("\rHuyOS> ");
                uart_puts(cli_buffer);
            }
            else
            {
                uart_puts("\rHuyOS> ");
            }
        }
        // Handle regular characters
        else if (c != '\n')
        {
            cli_buffer[index++] = c;
        }
        else
        {
            // Handle enter key
            cli_buffer[index] = '\0';

            // Save the command into history if it is not empty
            if (index > 0)
            {
                strcpy(cmd_history[history_index % MAX_HISTORY], cli_buffer);
                history_index++;
                // Wrap around if history is full
                if (history_index >= MAX_HISTORY)
                {
                    history_index = 0;
                }
            }
            current_index = history_index;
            uart_puts("\n");

            if (index > 0)
            {
                execute_command(cli_buffer);
            }
            else
            {
                uart_puts("");
            }
            index = 0;
            // Display prompt for the next command
            display_prompt();
        }
    }
}

// Main function
void main()
{
    setcolor("blue", NULL);
    uart_init();
    print_welcome_message(); // welcome message
    display_prompt();

    while (1)
    {
        cli();
    }
}

int strlen(const char *str)
{
    const char *s = str;
    while (*s)
        ++s;
    return s - str;
}

int strcmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int strncmp(const char *str1, const char *str2, int n)
{
    while (n && *str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
        n--;
    }
    if (n == 0)
        return 0;
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}
char *strtok(char *str, const char *delim)
{
    static char *next_token = NULL;
    char *token_start;

    if (str == NULL)
    {
        str = next_token;
    }

    if (str == NULL || *str == '\0')
    {
        next_token = NULL;
        return NULL;
    }

    token_start = str + strspn(str, delim);
    if (*token_start == '\0')
    {
        next_token = NULL;
        return NULL;
    }

    next_token = token_start + strcspn(token_start, delim);
    if (*next_token == '\0')
    {
        next_token = NULL;
    }
    else
    {

        *next_token = '\0';
        next_token++;
    }

    return token_start;
}

int strspn(const char *str, const char *set)
{
    int len = 0;
    while (*str && strchr(set, *str))
    {
        str++;
        len++;
    }
    return len;
}

int strcspn(const char *str, const char *set)
{
    int len = 0;
    while (*str && !strchr(set, *str))
    {
        str++;
        len++;
    }
    return len;
}

char *strchr(const char *str, int c)
{
    while (*str)
    {
        if (*str == (char)c)
        {
            return (char *)str;
        }
        str++;
    }
    if (*str == (char)c)
    {
        return (char *)str;
    }
    return NULL;
}
