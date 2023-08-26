#include "lcd.h"

// spi: https://mryslab.github.io/telemetrix-rpi-pico/spi/

void lcd_write_cmd(uint8_t cmd)
{
    gpio_put(LCD_PIN_DC, 0);
    spi_write_blocking(LCD_SPI_PORT, &cmd, 1);
}

void lcd_write_data_8bit(uint8_t data)
{
    gpio_put(LCD_PIN_DC, 1);
    spi_write_blocking(LCD_SPI_PORT, &data, 1);
}

void lcd_write_data_16bit(uint16_t data)
{
    uint8_t buff[] = {data >> 8, data & 0xFF};
    gpio_put(LCD_PIN_DC, 1);
    spi_write_blocking(LCD_SPI_PORT, buff, 2);
}

void lcd_write_ndata_8bit(uint8_t* data, uint16_t len)
{
    gpio_put(LCD_PIN_DC, 1);
    spi_write_blocking(LCD_SPI_PORT, data, len);
}

void lcd_write_ndata_16bit(uint16_t* data, uint16_t len)
{
    uint8_t* p = (uint8_t*)data;
    gpio_put(LCD_PIN_DC, 1);

    while (len--) {
        spi_write_blocking(LCD_SPI_PORT, p + 1, 1);
        spi_write_blocking(LCD_SPI_PORT, p, 1);
        p += 2;
    }
}

void lcd_reset(void)
{
#ifdef LCD_PIN_RST
    gpio_put(LCD_PIN_RST, 0);
    sleep_ms(100);
    gpio_put(LCD_PIN_RST, 1);
    sleep_ms(100);
#endif
}

void lcd_init(uint8_t* init_cmd)
{
    // gpio

    gpio_set_function(LCD_PIN_CLK, GPIO_FUNC_SPI);
    gpio_set_function(LCD_PIN_DIN, GPIO_FUNC_SPI);

    gpio_init(LCD_PIN_DC);
    gpio_set_dir(LCD_PIN_DC, 1);
    gpio_put(LCD_PIN_DC, 0);

#ifdef LCD_PIN_RST
    gpio_init(LCD_PIN_RST);
    gpio_set_dir(LCD_PIN_RST, 1);
#endif

#ifdef LCD_PIN_CS
    gpio_init(LCD_PIN_CS);
    gpio_set_dir(LCD_PIN_CS, 1);
    gpio_put(LCD_PIN_CS, 1);
#endif

#ifdef LCD_PIN_BL
    // backlight
    gpio_init(LCD_PIN_BL);
    gpio_set_dir(LCD_PIN_BL, GPIO_OUT);
    gpio_put(LCD_PIN_BL, 1);  // 1:off
#endif

    // spi: 40MHz
    spi_init(LCD_SPI_PORT, 40e6);
    // Set SPI format spi, databits, polarity (CPOL), phase (CPHA)
    // spi_set_format(LCD_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // hardware reset
    lcd_reset();

    // init cmd
    uint8_t* p = init_cmd;
    uint8_t  args_cnt;

    lcd_select();

    while (*p) {
        lcd_write_cmd(*p++ /* cmd */);
        switch (*p) {
            case 0: {
                ++p;
                break;
            }
            case LCD_CMD_WAIT_MS: {
                ++p;
                sleep_ms(*p++);
                break;
            }
            case LCD_CMD_WAIT_US: {
                ++p;
                sleep_us(*p++);
                break;
            }
            default: {
                args_cnt = *p++;
                lcd_write_ndata_8bit(p, args_cnt);
                p += args_cnt;
                break;
            }
        }
    }

    lcd_deselect();

#ifdef LCD_PIN_BL
    gpio_put(LCD_PIN_BL, 0);  // BL on
#endif
}

void lcd_set_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    lcd_write_cmd(LCD_CASET);
    lcd_write_data_16bit(x + LCD_XSTART);
    lcd_write_data_16bit(x + width - 1 + LCD_XSTART);

    lcd_write_cmd(LCD_RASET);
    lcd_write_data_16bit(y + LCD_YSTART);
    lcd_write_data_16bit(y + height - 1 + LCD_YSTART);

    lcd_write_cmd(LCD_RAMWR);
}

void lcd_flush(uint16_t* buffer, uint16_t width, uint16_t height)
{
    lcd_set_window(0, 0, width, height);
    lcd_write_ndata_16bit(buffer, width * height);
}

void lcd_select(void)
{
#ifdef LCD_PIN_CS
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_PIN_CS, 0);
    asm volatile("nop \n nop \n nop");
#endif
}
void lcd_deselect(void)
{
#ifdef LCD_PIN_CS
    asm volatile("nop \n nop \n nop");
    gpio_put(LCD_PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
#endif
}
