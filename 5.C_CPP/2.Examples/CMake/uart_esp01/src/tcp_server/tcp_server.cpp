#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "espat_wifi.h"

// current message
uint8_t m_id          = 0;
char    m_msg[64 + 1] = {0};

void tcp_server_send_data(int id, const char* dat)
{
    at_send_cmd("AT+CIPSEND=%d,%d", id, strlen(dat));
    if (at_recv_ack(">", AT_TIME_2S)) {
        at_puts(dat);
    }
}

bool tcp_server_recv_data()
{
    uint8_t i = 0;

    char *p1, *p2;

    while (at_readable_us(AT_TIME_2MS)) {
        m_msg[i] = at_getc();
        if (++i == 64) break;
    }

    if (i > 0) {
        m_msg[i] = '\0';

        printf(m_msg);
        printf("\r\n");

        p1 = strstr(m_msg, "+IPD");

        if (p1 != NULL) {
            
            p1 += 5;  // +IPD,
            *(p2 = strstr(p1, ",")) = '\0';
            m_id = atoi(p1);  // get id

            p1 = strstr(p2 + 1, ":") + 1;
            strcpy(m_msg, p1);  // get msg

            return true;
        }
    }

    return false;
}

int main()
{
    stdio_init_all();

    wifi_init();
    sleep_ms(3000);

    at_send_cmd("+++");  // 退出透传
    sleep_ms(1000);
    while (at_readable()) {
        at_getc();
    }

    at_send_recv("AT", "OK", AT_TIME_2S);
    at_send_recv("AT+CWMODE=3", "OK", AT_TIME_2S);

    at_send_cmd("AT+CWJAP=\"%s\",\"%s\"", wifi_ssid, wifi_pwd);
    at_recv_ack("OK", AT_TIME_10S);

    at_send_recv("AT+CIPMUX=1", "OK", AT_TIME_2S);

    at_send_cmd("AT+CIPSERVER=1,%s", local_port);
    at_recv_ack("OK", AT_TIME_2S);

    at_send_recv("AT+CIFSR", "OK", AT_TIME_2S);

    while (true) {
        while (tcp_server_recv_data()) {
            tcp_server_send_data(m_id, m_msg);
        }
    }
}
