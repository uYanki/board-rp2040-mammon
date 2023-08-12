#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "bsp_conf.h"
#include "cmds.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

static shell_status_t shell_cmd_led(int argc, char** argv);
static shell_status_t shell_cmd_version(int argc, char** argv);

cmd_t cmds[] = {
    {
     .cmd  = "led",
     .func = shell_cmd_led,
     .help = "<index:0><on/off/toggle> (Sets the built-in LED to the supplied state)",
     },
    {
     .cmd  = "version",
     .func = shell_cmd_version,
     .help = "(Returns the Build Versions & Number)",
     }
};

shell_t shell = {
    .println = shell_println,
    .cmd_tbl = cmds,
    .cmd_cnt = sizeof(cmds) / sizeof(cmd_t),
};

void shell_println(const char* string)
{
    puts(string);
}

static shell_status_t shell_cmd_led(int argc, char** argv)
{
    static uint8_t leds[] = {LED_PIN};

    if (argc != 3) {
        return SHELL_ERROR_INVALID_ARGS;
    }

    uint8_t index  = atoi(argv[1]);
    char*   action = argv[2];

    if (index >= ARRAY_SIZE(leds)) {
        return SHELL_ERROR_INVALID_ARGS;
    }

    if (!strcmp(action, "on")) {
        // led on
        gpio_put(leds[index], false);
        puts("LED[ON]");
    } else if (!strcmp(action, "off")) {
        // led off
        gpio_put(leds[index], true);
        puts("LED[OFF]");
    } else if (!strcmp(action, "toggle")) {
        // led toggle
        bool state = gpio_get(leds[index]);
        gpio_put(leds[index], !state);
        puts(state ? "LED[ON]" : "LED[OFF]");
    } else {
        return SHELL_ERROR_INVALID_ARGS;
    }

    return SHELL_SUCCESS;
}

static shell_status_t shell_cmd_version(int argc, char** argv)
{
    printf("Build Information: %d.%d.%d\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_NUMBER);
}
