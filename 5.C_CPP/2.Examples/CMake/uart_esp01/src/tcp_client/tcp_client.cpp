#include <stdio.h>

#include "espat_wifi.h"

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
    at_send_recv("AT+CWMODE=1", "OK", AT_TIME_2S);

    at_send_cmd("AT+CWJAP=\"%s\",\"%s\"", wifi_ssid, wifi_pwd);
    at_recv_ack("OK", AT_TIME_10S);

    at_send_recv("AT+CIPMUX=0", "OK", AT_TIME_2S);
    at_send_recv("AT+CIPMODE=1", "OK", AT_TIME_2S);

    at_send_cmd("AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    at_recv_ack("OK", AT_TIME_2S);

    at_send_recv("AT+CIPSEND", ">", AT_TIME_2S);

    at_println("Hello World !!!");
    at_println("ESP8266 TCP Client");

    int  i      = 0;    // counter
    char s[100] = {0};  // buffer
    while (true) {
        while (at_readable_us(AT_TIME_10MS)) {
            s[i++] = at_getc();
        }
        if (i > 0) {
            s[i] = '\0';
            printf("%s\r\n", s);
            at_puts(s);
            i = 0;
        }
    }
}
