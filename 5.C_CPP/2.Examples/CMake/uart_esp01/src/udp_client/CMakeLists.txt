add_executable(udp_client
        udp_client.cpp
        )

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(udp_client pico_stdlib espat)

# enable usb output, disable uart output
pico_enable_stdio_usb(udp_client 1)
pico_enable_stdio_uart(udp_client 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(udp_client)

# add url via pico_set_program_url
example_auto_set_url(udp_client)
