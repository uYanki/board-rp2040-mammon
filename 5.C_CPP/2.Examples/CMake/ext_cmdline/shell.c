#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "shell.h"

#define MAX_BUFFER_SIZE 80
#define CMD_TERMINATOR  '\r'

static uint8_t  buffer[MAX_BUFFER_SIZE + 1];
static uint8_t* buffer_ptr;

#define MAX_HISTORY 10
static uint8_t s_History[MAX_HISTORY][MAX_BUFFER_SIZE];
static int     s_HistoryIndex = 0;

static uint8_t cmd_buffer[MAX_BUFFER_SIZE];
static uint8_t cmd_pending;

const char shell_prompt[]       = ">> ";
const char shell_unrecognized[] = "[Error] Command not recognized.";

shell_status_t shell_cycle()
{
    int c = getchar_timeout_us(0);
    if (c != PICO_ERROR_TIMEOUT) {
        shell_put(c);
    }

    if (!cmd_pending) {
        return SHELL_IDLE;
    }

    uint8_t* p = cmd_buffer;
    while (*p) {
        *p = tolower(*p);
        ++p;
    }

    cmd_pending  = 0;
    uint8_t argc = 0;
    char*   argv[30];

    // puts((char*)cmd_buffer);
    /* Get the first token (cmd name) */
    argv[argc]         = strtok((char*)cmd_buffer, " \r");
    shell_status_t ret = SHELL_SUCCESS;
    /* Walk through the other tokens (parameters) */
    while ((argv[argc] != NULL) && (argc < 30)) {
        argv[++argc] = strtok(NULL, " \r");
    }

    if (argc == 0) {
        /* nothing to do */
    } else if (strcmp("help", argv[0]) == 0) {
        puts("");
        puts("+————————————————————+");
        puts("|  LIST OF COMMANDS  |");
        puts("+————————————————————+");
        for (size_t i = 0; i < shell.cmd_cnt; i++) {
            printf(shell.cmd_tbl[i].cmd);
            printf("\t: ");
            puts((char*)shell.cmd_tbl[i].help); /* Print the shell help */
        }
        puts("");
    } else {
        ret = SHELL_ERROR_CMD_NOT_FOUND;
        /* Search the command table for a matching command, using argv[0]
         * which is the command name. */
        for (size_t i = 0; i < shell.cmd_cnt; i++) {
            if (shell.cmd_tbl[i].cmd == NULL)
                break;
            if (strcmp(argv[0], shell.cmd_tbl[i].cmd) == 0) {
                /* Found a match, execute the associated function. */
                ret = shell.cmd_tbl[i].func(argc, argv);
                if (ret == SHELL_ERROR_INVALID_ARGS) {
                    puts("[ERROR] Invalid Arguments. Usage:");
                    puts(shell.cmd_tbl[i].help);
                }
            }
        }
    }

    if (ret == SHELL_ERROR_CMD_NOT_FOUND)
        puts((char*)shell_unrecognized);

    printf((char*)shell_prompt); /* Print the shell prompt to the user. */

    return ret;
}

void shell_init(void)
{
    buffer_ptr = buffer;

    cmd_pending = 0;

    /* Print the shell prompt. */
    printf((char*)shell_prompt);
}

/* ANSI Terminal manipulation */
#define MOVE_TO_LINE_START  "\r"
#define CLEAR_LINE          "\33[2K\r"
#define MAX_ESCAPE_SEQUENCE 2
static bool s_bEsc                             = false;
static char s_EscSequence[MAX_ESCAPE_SEQUENCE] = {};
static int  s_EscCount                         = 0;
static int  s_HistoryBack                      = 0;

static void ShellEscHandling(void)
{
    if (strcmp(s_EscSequence, "[A") == 0) {
        if (s_HistoryIndex) {
            s_HistoryBack = (s_HistoryBack + 1) % MAX_HISTORY;
            int curr      = (s_HistoryIndex - s_HistoryBack) % MAX_HISTORY;

            int len = strlen((char*)s_History[curr]);

            printf((char*)CLEAR_LINE);
            printf(shell_prompt);
            printf((char*)s_History[curr]);
            memcpy((char*)buffer, (char*)s_History[curr], len);
            buffer_ptr = buffer + len;
        }
    }
    if (strcmp(s_EscSequence, "[B") == 0) {
        s_HistoryBack = (s_HistoryBack - 1) % MAX_HISTORY;
        int curr      = (s_HistoryIndex - s_HistoryBack) % MAX_HISTORY;

        int len = strlen((char*)s_History[curr]);

        printf((char*)CLEAR_LINE);
        printf(shell_prompt);
        printf((char*)s_History[curr]);
        memcpy((char*)buffer, (char*)s_History[curr], len);
        buffer_ptr = buffer + len;
    }
    if (strcmp(s_EscSequence, "[C") == 0) {
        // ConsoleTransmit((uint8_t *)"Right Arrow",12);
    }
    if (strcmp(s_EscSequence, "[D") == 0) {
        // ConsoleTransmit((uint8_t *)"Left Arrow",11);
    }
}

void shell_put(char c)
{
    if (s_bEsc) {
        s_EscSequence[s_EscCount++] = c;
        if (s_EscCount >= MAX_ESCAPE_SEQUENCE) {
            ShellEscHandling();
            s_EscCount = 0;
            s_bEsc     = false;
        }
        return;
    }

    switch (c) {
        case 022: /* CTRL-R */
            printf((char*)CLEAR_LINE);
            printf((char*)buffer);
            break;
        case 025:                /* CTRL-U */
            buffer_ptr = buffer; /* Reset buf_ptr to beginning. */
            printf(CLEAR_LINE);
            break;
        case 033:
            s_bEsc = true;
            break;
        case '\b':
            /* Backspace. Delete character. */
            if (buffer_ptr > buffer) {
                buffer_ptr--;
                printf("\b \b");
            }
            break;
        case CMD_TERMINATOR:
            if (!cmd_pending) {
                *buffer_ptr = '\0'; /* Terminate the msg and reset the msg ptr. */
                int curr    = s_HistoryIndex % MAX_HISTORY;
                int len     = strnlen((const char*)buffer, MAX_BUFFER_SIZE);
                if (strncmp((char*)s_History[curr], (char*)buffer, MAX_BUFFER_SIZE)) /* if same as the last command no need to update */
                {
                    memcpy((char*)s_History[curr], (char*)buffer, len);
                    s_History[curr][len] = 0;                  /* NULL terminate */
                    if (buffer_ptr - buffer) ++s_HistoryIndex; /* increment history index only if not an empty string */
                }
                memcpy((char*)cmd_buffer, (char*)buffer, len);
                cmd_buffer[len] = 0;
                cmd_pending     = 1;
                buffer_ptr      = buffer; /* Reset buf_ptr to beginning. */
            }
            putchar(c);
            putchar('\n');
            s_HistoryBack = 0;
            break;

        default:
            if ((buffer_ptr - buffer) < MAX_BUFFER_SIZE) {
                *buffer_ptr++ = c;
                *buffer_ptr   = 0;
                putchar(c);
            }
    }
}