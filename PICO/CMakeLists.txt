# git submodule add https://github.com/raspberrypi/pico-sdk.git deps/pico-sdk

cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

if (NOT PICO_SDK_PATH AND NOT DEFINED ENV{PICO_SDK_PATH})
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../pico-sdk" CACHE PATH "Path to the Raspberry Pi Pico SDK")
endif()

set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)
project(my_project C CXX ASM)

pico_sdk_init()

function(add_local_uf2_output TARGET)
    pico_get_output_name(${TARGET} output_name)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_LIST_DIR}/tools/bin_to_uf2.py
            ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.bin
            ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.uf2
            --base 0x10000000
            --family-id 0xE48BFF59
        VERBATIM
        BYPRODUCTS
            ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.uf2
        COMMENT "Generating UF2 for ${TARGET} from BIN"
    )
endfunction()

add_executable(hello_pico src/main.c)
add_executable(blink
    src/blink.c
    )
add_executable(hello
    src/hello.c
    )
add_executable(temp
    src/temp.c
    )
add_executable(buzzer
    src/buzzer.c
    )
add_executable(ultra
    src/ultra.c
    )

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/src/servo.c")
    add_executable(servo
        src/servo.c
    )
endif()
    

# 표준 라이브러리 연결
target_link_libraries(hello_pico pico_stdlib)
target_link_libraries(blink pico_stdlib)
target_link_libraries(hello pico_stdlib hardware_i2c)
target_link_libraries(temp pico_stdlib hardware_i2c)
target_link_libraries(buzzer pico_stdlib hardware_i2c hardware_pwm)
target_link_libraries(ultra pico_stdlib hardware_i2c)

if (TARGET servo)
    target_link_libraries(servo pico_stdlib hardware_pwm hardware_clocks)
endif()

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(blink pico_cyw43_arch_none)
endif()


pico_add_extra_outputs(hello_pico)
pico_add_extra_outputs(blink)
pico_add_extra_outputs(hello)
pico_add_extra_outputs(temp)
pico_add_extra_outputs(buzzer)
pico_add_extra_outputs(ultra)
add_local_uf2_output(hello_pico)
add_local_uf2_output(blink)
add_local_uf2_output(hello)
add_local_uf2_output(temp)
add_local_uf2_output(buzzer)
add_local_uf2_output(ultra)

if (TARGET servo)
    pico_add_extra_outputs(servo)
    add_local_uf2_output(servo)
endif()

# USB 출력을 활성화하려면 아래 주석 해제 (Serial 모니터용)
pico_enable_stdio_usb(hello_pico 1)
pico_enable_stdio_uart(hello_pico 0)
pico_enable_stdio_usb(hello 1)
pico_enable_stdio_uart(hello 0)
pico_enable_stdio_usb(temp 1)
pico_enable_stdio_uart(temp 0)
pico_enable_stdio_usb(buzzer 1)
pico_enable_stdio_uart(buzzer 0)
pico_enable_stdio_usb(ultra 1)
pico_enable_stdio_uart(ultra 0)

if (TARGET servo)
    pico_enable_stdio_usb(servo 1)
    pico_enable_stdio_uart(servo 0)
endif()

add_subdirectory(src/mqtt)
