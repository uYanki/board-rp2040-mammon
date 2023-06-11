/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

*/

#define PIN_I2C0_SDA 20
#define PIN_I2C0_SCL 21

// By default these devices  are on bus address 0x68
static int addr = 0x68;

static void mpu6050_detect()
{
    uint8_t buf;

    do {
        buf = 0x75;
        i2c_write_blocking(i2c0, addr, &buf, 1, false);
        i2c_read_blocking(i2c0, addr, &buf, 1, false);
        printf("who am i = 0x%02x", buf);
        sleep_ms(500);
    } while (buf != 0x68);
}

static void mpu6050_reset()
{
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0};
    i2c_write_blocking(i2c0, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t* temp)
{
    uint8_t buf[14];

    uint8_t val = 0x3B;
    i2c_write_blocking(i2c0, addr, &val, 1, true);
    i2c_read_blocking(i2c0, addr, buf, 14, false);

    accel[0] = (buf[0] << 8) | buf[1];
    accel[1] = (buf[2] << 8) | buf[3];
    accel[2] = (buf[4] << 8) | buf[5];
    temp[0]  = (buf[6] << 8) | buf[7];
    gyro[0]  = (buf[8] << 8) | buf[9];
    gyro[1]  = (buf[10] << 8) | buf[11];
    gyro[2]  = (buf[12] << 8) | buf[13];

    return;
}

int main()
{
    stdio_init_all();

    printf("Hello, MPU6050! Reading raw data from registers...\n");

    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(PIN_I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C0_SDA);
    gpio_pull_up(PIN_I2C0_SCL);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PIN_I2C0_SDA, PIN_I2C0_SCL, GPIO_FUNC_I2C));

    mpu6050_detect();
    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;

    while (1) {
        mpu6050_read_raw(acceleration, gyro, &temp);

        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        printf("Temp. = %f\n", (temp / 340.0) + 36.53);  // deg C

        sleep_ms(100);
    }

    return 0;
}
