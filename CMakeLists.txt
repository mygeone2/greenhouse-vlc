if (TARGET tinyusb_device)
    add_executable(hello_adc
                hello_adc.cpp
            )

    # pull in common dependencies
    target_link_libraries(hello_adc pico_stdlib hardware_adc hardware_i2c)
    
    # enable usb output, disable uart output
    pico_enable_stdio_usb(hello_adc 1)
    pico_enable_stdio_uart(hello_adc 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(hello_adc)

    # add url via pico_set_program_url
    example_auto_set_url(hello_adc)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_adc because TinyUSB submodule is not initialized in the SDK")
endif()
