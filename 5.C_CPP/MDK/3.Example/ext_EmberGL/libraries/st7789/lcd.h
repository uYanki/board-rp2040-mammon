#ifndef __LCD_H__
#define __LCD_H__

#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/spi.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "lcd_conf.h"

#define RGB565_BLACK   0x0000
#define RGB565_WHITE   0xFFFF
#define RGB565_RED     0xF800
#define RGB565_GREEN   0x07E0
#define RGB565_BLUE    0x001F
#define RGB565_CYAN    0x07FF
#define RGB565_MAGENTA 0xF81F
#define RGB565_YELLOW  0xFFE0
#define RGB565_ORANGE  0xFC00

void lcd_write_cmd(uint8_t cmd);
void lcd_write_data_8bit(uint8_t data);
void lcd_write_data_16bit(uint16_t data);
void lcd_write_ndata_8bit(uint8_t* data, uint16_t len);
void lcd_write_ndata_16bit(uint16_t* data, uint16_t len);

void lcd_init(const uint8_t* init_cmd);
void lcd_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void lcd_flush(uint16_t* buffer, uint16_t width, uint16_t height);

void lcd_select(void);
void lcd_deselect(void);

#ifdef __cplusplus
}
#endif

#endif