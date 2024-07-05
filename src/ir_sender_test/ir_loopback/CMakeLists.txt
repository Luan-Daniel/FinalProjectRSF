if (TARGET tinyusb_device)

  add_executable (${EXECUTABLE_NAME} ir_loopback.c)

  # link the executable using the IR transmit and receive libraries
  #
  target_link_libraries(${EXECUTABLE_NAME} LINK_PUBLIC
    pico_stdlib
    hardware_pio
    nec_transmit_library
  )

  pico_enable_stdio_usb(${EXECUTABLE_NAME} 1)
  pico_enable_stdio_uart(${EXECUTABLE_NAME} 0)

  pico_add_extra_outputs(${EXECUTABLE_NAME})

elseif(PICO_ON_DEVICE)
  message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()