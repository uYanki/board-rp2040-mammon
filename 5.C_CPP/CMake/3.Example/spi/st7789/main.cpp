#include <stdio.h>

#include "lcd.h"

#define w 30
#define h 60

uint16_t buffer[w * h];

int main()
{
    stdio_init_all();

    for (uint16_t i = 0; i < w * h; ++i) buffer[i] = RGB565_RED;

    lcd_init((uint8_t*)st7789_init_cmd);

    lcd_select();
    lcd_set_window(10, 30, w, h);
    lcd_write_ndata_16bit(buffer, w * h);
    lcd_deselect();

    while (1) {
    }

    return 0;
}