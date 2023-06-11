#define PIN_LED         0  // 0 = on, 1 = off
#define PIN_RGBLED      1  // *2
#define PIN_BUZZER      2

#define PIN_WIFI_EN     3
#define PIN_WIFI_TX     4
#define PIN_WIFI_RX     5

#define PIN_KEY1        7
#define PIN_KEY2        13
#define PIN_KEY3        12

#define PIN_PDM_DATA    10
#define PIN_PDM_CLK     11

#define PIN_I2C0_SDA    20
#define PIN_I2C0_SCL    21

#define MPU6050_ADDRESS 0x44
#define I2C_MPU6050     i2c0
#define PIN_MPU6050_INT 15
#define PIN_MPU6050_SDA PIN_I2C0_SDA
#define PIN_MPU6050_SCL PIN_I2C0_SCL

#define SHT30_ADDRESS   0x68
#define I2C_SHT30       i2c0
#define PIN_SHT30_SDA   PIN_I2C0_SDA
#define PIN_SHT30_SCL   PIN_I2C0_SCL

#define PIN_SPI0_MISO   16
#define PIN_SPI0_SCLk   18
#define PIN_SPI0_MOSI   19

#define SPI_FONT        spi0
#define PIN_FONT_MISO   PIN_SPI0_MISO
#define PIN_FONT_SCLk   PIN_SPI0_SCLk
#define PIN_FONT_MOSI   PIN_SPI0_MOSI
#define PIN_FONT_CS     17

#define SPI_TF          spi0
#define PIN_TF_MISO     PIN_SPI0_MISO
#define PIN_TF_SCLk     PIN_SPI0_SCLk
#define PIN_TF_MOSI     PIN_SPI0_MOSI
#define PIN_TF_CS       14

#define SPI_LCD         spi0
#define PIN_LCD_MISO    PIN_SPI0_MISO
#define PIN_LCD_SCLk    PIN_SPI0_SCLk
#define PIN_LCD_MOSI    PIN_SPI0_MOSI
#define PIN_LCD_BL      22  // 0 = on, 1 = off
#define PIN_LCD_RST     23
#define PIN_LCD_DC      24
#define PIN_LCD_CS      25

#define PIN_ADC0        26
#define PIN_ADC1        27
#define PIN_ADC2        28
#define PIN_ADC3        29

#define PIN_ADC_RT      PIN_ADC2
#define PIN_ADC_VBAT    PIN_ADC3
