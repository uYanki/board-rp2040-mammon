#ifndef __LCD_CONF_H__
#define __LCD_CONF_H__

#include <stdint.h>

#include "st7789.h"

///< port
#define LCD_SPI_PORT spi0
#define LCD_PIN_CS   25
#define LCD_PIN_CLK  18
#define LCD_PIN_DIN  19
#define LCD_PIN_DC   24
#define LCD_PIN_RST  23
#define LCD_PIN_BL   22

///< rotation
#define LCD_ROT_0    0
#define LCD_ROT_1    1
#define LCD_ROT_2    2
#define LCD_ROT_3    3

#define LCD_ROTATION LCD_ROT_1

// 1.14inch
#if LCD_ROTATION == LCD_ROT_0
#define LCD_MADCTL (ST7789_MADCTL_RGB | ST7789_MADCTL_MY)
#define LCD_WIDTH  135
#define LCD_HEIGHT 240
#define LCD_XSTART 53
#define LCD_YSTART 40
#elif LCD_ROTATION == LCD_ROT_1
#define LCD_MADCTL (ST7789_MADCTL_RGB | ST7789_MADCTL_MY | ST7789_MADCTL_MV)
#define LCD_WIDTH  240
#define LCD_HEIGHT 135
#define LCD_XSTART 40
#define LCD_YSTART 52
#elif LCD_ROTATION == LCD_ROT_2
#define LCD_MADCTL (ST7789_MADCTL_RGB | ST7789_MADCTL_MX)
#define LCD_WIDTH  135
#define LCD_HEIGHT 240
#define LCD_XSTART 52
#define LCD_YSTART 40
#elif LCD_ROTATION == LCD_ROT_3
#define LCD_MADCTL (ST7789_MADCTL_RGB | ST7789_MADCTL_MX | ST7789_MADCTL_MV)
#define LCD_WIDTH  240
#define LCD_HEIGHT 135
#define LCD_XSTART 40
#define LCD_YSTART 53
#endif

//

///< cmd
#define LCD_CASET       ST7789_CASET
#define LCD_RASET       ST7789_RASET
#define LCD_RAMWR       ST7789_RAMWR

///< delay
#define LCD_CMD_WAIT_MS 0xFF
#define LCD_CMD_WAIT_US 0xFE

static const uint8_t st7789_init_cmd[] = {
    // ST7789_SWRESET, LCD_CMD_WAIT_MS, 50,              // soft reset

    ST7789_COLMOD, 1, 0x55,                           // color mode = rgb565

    ST7789_PORCTRL, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,  // porch control
    ST7789_MADCTL, 1, LCD_MADCTL,                     // display rotation
    ST7789_GCTRL, 1, 0x35,                            // gate control
    ST7789_VCOMS, 1, 0x19,                            // default 0.75v for 0x20
    ST7789_LCMCTRL, 1, 0x2C,
    ST7789_VDVVRHEN, 1, 0x01,
    ST7789_VRHS, 1, 0x12,    // +-4.45v, defalut +-4.1v for 0x0B
    ST7789_VDVSET, 1, 0x20,
    ST7789_FRCTR2, 1, 0x0F,  // frame rate = 60hz
    ST7789_PWCTRL1, 2, 0xA4, 0XA1,
    ST7789_PVGAMCTRL, 14, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23,
    ST7789_NVGAMCTRL, 14, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23,

    ST7789_INVON, 0,   // inverst color
    ST7789_SLPOUT, 0,  // LCD_CMD_WAIT_MS, 50,  // out of sleep mode
    ST7789_NORON, 0,   // normal display on
    ST7789_DISPON, 0,  // LCD_CMD_WAIT_MS, 50,  // screen turned on

    0                  // end
};

#endif