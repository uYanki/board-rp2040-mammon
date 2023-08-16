add_library(espat
        espat_wifi.c
        )

include_directories(.)

target_link_libraries(espat pico_stdlib hardware_gpio hardware_uart)
