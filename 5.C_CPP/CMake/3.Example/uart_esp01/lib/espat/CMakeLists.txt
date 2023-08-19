add_library(espat
        espat_wifi.c
        )

target_include_directories(espat PUBLIC .)

target_link_libraries(espat pico_stdlib hardware_gpio hardware_uart)
