#include <stdio.h>

#include "pico/stdlib.h"

extern void lua_run(void);

int main()
{
    stdio_init_all();

    lua_run();

    while (true) {
        sleep_ms(100);
    }
}
