#pragma once

#define CONFIG_PID_MODULE    1
#define CONFIG_LED_MODULE    1
#define CONFIG_USING_CMDWORD 1

#define MB_UART              uart1
#define MB_UART_IRQ          UART1_IRQ
#define MB_UART_BAUD         9600  // RS485,Modbus
#define MB_DE_PIN            7
#define MB_TX_PIN            8
#define MB_RX_PIN            9
#define USB_ITF_MB           1

#define LED_PIN              PICO_DEFAULT_LED_PIN
