#include <stdarg.h>
#include <string.h>

#include "espat_wifi.h"

// end with '\0'
static char m_buf[128 + 1] = {0};

void wifi_init(void)
{
    gpio_init(WIFI_PIN_EN);
    gpio_set_dir(WIFI_PIN_EN, GPIO_OUT);

    gpio_put(WIFI_PIN_EN, 1);  // on
    sleep_ms(1000);

    uart_init(WIFI_UART_PORT, WIFI_UART_BAUD);
    gpio_set_function(WIFI_PIN_TX, GPIO_FUNC_UART);
    gpio_set_function(WIFI_PIN_RX, GPIO_FUNC_UART);
}

void at_printf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    vsnprintf(m_buf, sizeof(m_buf) - 1, str, args);
    va_end(args);

    at_puts(m_buf);
}

void at_send_cmd(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    vsnprintf(m_buf, sizeof(m_buf) - 1, str, args);
    va_end(args);

    at_puts(m_buf);
    at_puts("\r\n");
}

bool at_recv_ack(const char* ack, uint64_t timeout)
{
    static char m_buf[64 + 1] = {0};

    uint8_t  n = 0;  // counter
    uint64_t t = time_us_64() + timeout;

    while (time_us_64() < t) {
        while (at_readable_us(AT_TIME_10MS)) {
            m_buf[n] = at_getc();
            if (++n == 64) break;
        }
        if (n > 0) {
            m_buf[n] = '\0';
#if 1
            printf(m_buf);  // log
            printf("\r\n");
#endif
            if (strstr(m_buf, ack)) {
                return true;
            } else {
                n = 0;
            }
        }
    }

    return false;
}

bool at_send_recv(const char* cmd, const char* ack, uint64_t timeout)
{
    at_send_cmd(cmd);
    return at_recv_ack(ack, timeout);
}