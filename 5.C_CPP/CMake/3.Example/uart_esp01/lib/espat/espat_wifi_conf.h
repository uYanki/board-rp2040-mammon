#pragma once

#define WIFI_PIN_EN    3
#define WIFI_PIN_TX    4
#define WIFI_PIN_RX    5
#define WIFI_UART_PORT uart1
#define WIFI_UART_BAUD 115200

// wifi
static const char* wifi_ssid = "uYanki";
static const char* wifi_pwd  = "12345678";

// for tcp
static const char* server_ip   = "192.168.43.1";
static const char* server_port = "8888";

// for udp
#define remote_ip   server_ip
#define remote_port server_port
static const char local_port[] = "8080";
