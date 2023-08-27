import esp_at_uart
from machine import Pin, UART

# Note: AT http is only available on ESP32/ESP32-S2 AT firmware.

TEST_AP_SSID = "uYanki"  # YOUR_AP_SSID
TEST_AP_PASS = "12345678"  # YOUR_AP_PWD

wifi_en = Pin(3, Pin.OUT)
wifi_en.value(1)
uart = UART(1, baudrate=115200, tx=Pin(4), rx=Pin(5))
esp = esp_at_uart.ESPCHIP(uart)
# esp = esp_at_uart.ESPCHIP(1, 9600)


def connect(ssid, pwd):
    esp.set_mode(esp_at_uart.WIFI_MODES["STATION"])
    if not esp.get_mode() == esp_at_uart.WIFI_MODES["STATION"]:
        return False

    print('Connecting WLAN...')
    return esp.connect(ssid, pwd)


def run():
    print('Start')
    if esp.test():
        print('Ready')
        ok = connect(TEST_AP_SSID, TEST_AP_PASS)
        if ok:
            print('Connected:', esp.get_station_ip())
            res = esp.http_request("http://httpbin.org/get", debug=False)
            print('Received: ', res['size'], 'bytes')
            print('Body:')
            print(res['data'].decode('utf-8'))
        else:
            print('WiFi Failed')
    else:
        print('AT Failed!')


run()
