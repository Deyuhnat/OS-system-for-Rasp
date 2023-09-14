// -----------------------------------main.c -------------------------------------

#include "uart1.h"
#define MAX_CMD_SIZE 100
#define MAX_HISTORY 10
#define NULL ((void *)0)

int strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int n);
char *strtok(char *str, const char *delim);
int strspn(const char *str, const char *set);
int strcspn(const char *str, const char *set);
char *strchr(const char *str, int c);

char cmd_history[MAX_HISTORY][MAX_CMD_SIZE];
int history_index = 0;
int current_index = 0;

void print_welcome_message() {
    uart_puts("d88888b d88888b d88888b d888888b .d888b.   j88D  .d888b.  .d88b. \n");
    uart_puts("88'     88'     88'     `~~88~~' VP  `8D  j8~88  88' `8D .8P  88.\n");
    uart_puts("88ooooo 88ooooo 88ooooo    88       odD' j8' 88  `V8o88' 88  d'88\n");
    uart_puts("88~~~~~ 88~~~~~ 88~~~~~    88     .88'   V88888D    d8'  88 d' 88\n");
    uart_puts("88.     88.     88.        88    j88.        88    d8'   `88  d8'\n");
    uart_puts("Y88888P Y88888P Y88888P    YP    888888D     VP   d8'     `Y88P' \n");
	uart_puts("\n");
	uart_puts("\n");
	uart_puts("db   d8b   db d88888b db       .o88b.  .d88b.  .88b  d88. d88888b   d888888b  .d88b.    d8888b.  .d8b.  d8888b. d88888b    .d88b.  .d8888. \n");
	uart_puts("88   I8I   88 88'     88      d8P  Y8 .8P  Y8. 88'YbdP`88 88'       `~~88~~' .8P  Y8.   88  `8D d8' `8b 88  `8D 88'       .8P  Y8. 88'  YP \n");
	uart_puts("88   I8I   88 88ooooo 88      8P      88    88 88  88  88 88ooooo      88    88    88   88oooY' 88ooo88 88oobY' 88ooooo   88    88 `8bo.  \n");
	uart_puts("Y8   I8I   88 88~~~~~ 88      8b      88    88 88  88  88 88~~~~~      88    88    88   88~~~b. 88~~~88 88`8b   88~~~~~   88    88   `Y8b.\n");
	uart_puts("`8b d8'8b d8' 88.     88booo. Y8b  d8 `8b  d8' 88  88  88 88.          88    `8b  d8'   88   8D 88   88 88 `88. 88.       `8b  d8' db   8D \n");
	uart_puts(" `8b8' `8d8'  Y88888P Y88888P  `Y88P'  `Y88P'  YP  YP  YP Y88888P      YP     `Y88P'    Y8888P' YP   YP 88   YD Y88888P    `Y88P'  `8888Y' \n");
    uart_puts("By Nguyen Tan Huy - s3864185\n");
}

char *commands[] = {
    "help",
    "clear",
    "setcolor",
    "showinfo"};
int num_commands = sizeof(commands) / sizeof(commands[0]);
const char *colors[] = {
    "BLACK",
    "RED",
    "GREEN",
    "YELLOW",
    "BLUE",
    "PURPLE",
    "CYAN",
    "WHITE"};

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
void uppercaseLetter(char *str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str = *str - ('a' - 'A');
        }
        str++;
    }
}

void setcolor(const char *textColor, const char *backgroundColor)
{
    char upperTextColor[MAX_CMD_SIZE] = {0};
    char upperBackgroundColor[MAX_CMD_SIZE] = {0};

    if (textColor) {
        strcpy(upperTextColor, textColor);
        uppercaseLetter(upperTextColor);
    }
    if (backgroundColor) {
        strcpy(upperBackgroundColor, backgroundColor);
        uppercaseLetter(upperBackgroundColor);
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

void display_prompt()
{
    uart_puts("HuyOS> ");
}
void execute_command(const char *cmd)
{
    if (strcmp(cmd, "help") == 0)
    {
        uart_puts("Available commands\n");
        uart_puts("help - show all commands available\n");
        uart_puts("clear - clear all element in terminal\n");
        uart_puts("setcolor - set color of text or background by using setcolor -t 'color' -b 'color' (You need to write 'help' in order to setcolor again)\n");
        uart_puts("showinfo - Show board revision and board MAC address\n");
    }
    else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 'e' && cmd[3] == 'a' && cmd[4] == 'r' && cmd[5] == '\0') {
        uart_puts("\033[H\033[J");
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
    } else {
        uart_puts("Unknown command\n");
    }
}

void cli()
{
    static char cli_buffer[MAX_CMD_SIZE];
    static int index = 0;
    char c;
    while (1)
    {
        c = uart_getc();
        uart_sendc(c); // echo the character

        if (c == '\t')
        { // Auto-completion
            for (int i = 0; i < num_commands; i++)
            {
                if (strncmp(cli_buffer, commands[i], strlen(cli_buffer)) == 0)
                {
                    strcpy(cli_buffer, commands[i]);
                    uart_puts(&cli_buffer[index]); // print remaining characters
                    index = strlen(cli_buffer);
                    break;
                }
            }
        }
        else if (c == '_')
        { // History UP
            if (current_index == history_index)
            {
                // If currently at the most recent command, save current input first
                strcpy(cmd_history[history_index % MAX_HISTORY], cli_buffer);
            }
            current_index = (current_index - 1 + MAX_HISTORY) % MAX_HISTORY; // Wrap around if needed
            strcpy(cli_buffer, cmd_history[current_index]);
            index = strlen(cli_buffer);
            uart_puts("\rHuyOS>             ");
            uart_puts("\rHuyOS> ");
            uart_puts(cli_buffer);
        }
        else if (c == '+')
        {                                                      // History DOWN
            current_index = (current_index + 1) % MAX_HISTORY; // Wrap around if needed
            if (current_index == history_index)
            {
                // If going back to the most recent command, retrieve the saved input
                strcpy(cli_buffer, cmd_history[current_index]);
            }
            else
            {
                strcpy(cli_buffer, cmd_history[current_index]);
            }
            index = strlen(cli_buffer);
            uart_puts("\rHuyOS>             "); // Clear line (add more spaces if needed)
            uart_puts("\rHuyOS> ");
            uart_puts(cli_buffer);
        }
        else if (c != '\n')
        {
            cli_buffer[index++] = c;
        }
        else
        {
            cli_buffer[index] = '\0';

            // Store in history
            if (index > 0)
            {
                strcpy(cmd_history[history_index % MAX_HISTORY], cli_buffer);
                history_index++;
                if (history_index >= MAX_HISTORY)
                {
                    history_index = 0; // wrap around
                }
            }
            current_index = history_index;
            uart_puts("\n");
            execute_command(cli_buffer);
            index = 0;
            display_prompt();
        }
    }
}

void main()
{
    uart_init();
    //uart_puts(welcome_message);
    print_welcome_message();
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

char *strcpy(char *dest, const char *src)
{
    char *save = dest;
    while ((*dest++ = *src++))
        ;
    return save;
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

    // If the input string is NULL, continue tokenizing the previous string
    if (str == NULL)
    {
        str = next_token;
    }

    // If the string is NULL or an empty string, return NULL
    if (str == NULL || *str == '\0')
    {
        next_token = NULL;
        return NULL;
    }

    // Tokenize and skip any leading delimiters
    token_start = str + strspn(str, delim);
    if (*token_start == '\0')
    {
        next_token = NULL;
        return NULL;
    }

    // Find the end of the token
    next_token = token_start + strcspn(token_start, delim);
    if (*next_token == '\0')
    {
        next_token = NULL;
    }
    else
    {
        // Replace the following delimiter with a null terminator
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
