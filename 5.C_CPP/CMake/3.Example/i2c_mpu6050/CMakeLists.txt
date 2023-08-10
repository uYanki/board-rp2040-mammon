cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(i2c_mpu6050 C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

if (TARGET tinyusb_device)

        add_executable(${PROJECT_NAME} main.c)

        # pull in common dependencies and additional i2c hardware support
        target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_i2c)

        # enable usb output, disable uart output
        pico_enable_stdio_usb(${PROJECT_NAME} 1)
        pico_enable_stdio_uart(${PROJECT_NAME} 0)

        # create map/bin/hex file etc.
        pico_add_extra_outputs(${PROJECT_NAME})

elseif(PICO_ON_DEVICE)
    message(WARNING "not building i2c_mpu6050 because TinyUSB submodule is not initialized in the SDK")
endif()



