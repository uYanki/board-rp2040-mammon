#include "cmds.h"
#include "shell.h"
#include "bsp_conf.h"

void bsp_init()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    gpio_init(BEEP_PIN);
    gpio_set_dir(BEEP_PIN, GPIO_OUT);
    gpio_put(BEEP_PIN, false);
}

int main()
{
    stdio_init_all();
    bsp_init();

    shell_init();
    while (1) {
        shell_cycle();
    }

    return 0;
}
