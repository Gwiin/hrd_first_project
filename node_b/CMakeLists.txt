cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w)
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../pico-sdk")

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(node_b C CXX ASM)

pico_sdk_init()

add_executable(node_b
    src/main.c
)

target_include_directories(node_b PRIVATE include)
target_link_libraries(node_b PRIVATE pico_stdlib hardware_pwm)

pico_enable_stdio_uart(node_b 0)
pico_enable_stdio_usb(node_b 1)
pico_add_extra_outputs(node_b)
