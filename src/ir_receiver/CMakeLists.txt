set(PROJECT_NAME "ir_receiver") # Same as parent directory
set(EXECUTABLE_NAME "receiver_program")


cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(${PROJECT_NAME})

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(nec_receive_library)
add_subdirectory(ir_loopback)