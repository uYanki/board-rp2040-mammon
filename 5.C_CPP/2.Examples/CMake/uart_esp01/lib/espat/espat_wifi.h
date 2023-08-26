#pragma once

#include <stdio.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "espat_wifi_conf.h"

#define AT_TIME_1US             (1)  // 1us

#define AT_TIME_1MS             (1000ull)           // 1ms
#define AT_TIME_2MS             (2 * AT_TIME_1MS)   // 2s
#define AT_TIME_5MS             (5 * AT_TIME_1MS)   // 5s
#define AT_TIME_10MS            (10 * AT_TIME_1MS)  // 10s

#define AT_TIME_1S              (1000000ull)       // 1s
#define AT_TIME_2S              (2 * AT_TIME_1S)   // 2s
#define AT_TIME_5S              (5 * AT_TIME_1S)   // 5s
#define AT_TIME_10S             (10 * AT_TIME_1S)  // 10s

////////////////////

#define at_puts(str)            uart_puts(WIFI_UART_PORT, str)
#define at_getc()               uart_getc(WIFI_UART_PORT)
#define at_readable()           uart_is_readable(WIFI_UART_PORT)
#define at_readable_us(timeout) uart_is_readable_within_us(WIFI_UART_PORT, timeout)

////////////////////

#define at_println(str, ...)    at_printf(str "\r\n", ##__VA_ARGS__)

////////////////////

void wifi_init(void);
void at_printf(const char* str, ...);
void at_send_cmd(const char* str, ...);
bool at_recv_ack(const char* ack, uint64_t timeout);
bool at_send_recv(const char* cmd, const char* ack, uint64_t timeout);

#ifdef __cplusplus
}
#endif
