add_executable(tcp_client
        tcp_client.cpp
        )

target_include_directories(tcp_client PRIVATE ${PROJECT_SOURCE_DIR}/lib/espat)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(tcp_client pico_stdlib espat)

# enable usb output, disable uart output
pico_enable_stdio_usb(tcp_client 1)
pico_enable_stdio_uart(tcp_client 0)
	
# create map/bin/hex file etc.
pico_add_extra_outputs(tcp_client)

# add url via pico_set_program_url
example_auto_set_url(tcp_client)
