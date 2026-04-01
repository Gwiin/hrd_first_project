cmake_minimum_required(VERSION 3.13)

if (NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set.")
endif ()

if (NOT PICO_BOARD)
    set(PICO_BOARD pico2_w CACHE STRING "Target board" FORCE)
endif ()

option(PICO_NO_PICOTOOL "Skip picotool fetch during local builds" OFF)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(node_b C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(NODE_B_WIFI_SSID "$ENV{WIFI_SSID}")
set(NODE_B_WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")
set(NODE_B_MQTT_SERVER "$ENV{MQTT_SERVER}")
set(NODE_B_MQTT_USERNAME "$ENV{MQTT_USERNAME}")
set(NODE_B_MQTT_PASSWORD "$ENV{MQTT_PASSWORD}")

add_executable(node_b
    src/tempservo_led.c
)

pico_generate_pio_header(node_b
    $ENV{PICO_SDK_PATH}/src/rp2_common/pico_status_led/ws2812.pio
)

target_include_directories(node_b PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(node_b PRIVATE
    pico_stdlib
    hardware_pio
    hardware_pwm
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
)

if (NODE_B_WIFI_SSID)
    target_compile_definitions(node_b PRIVATE
        WIFI_SSID=\"${NODE_B_WIFI_SSID}\"
    )
endif ()

if (NODE_B_WIFI_PASSWORD)
    target_compile_definitions(node_b PRIVATE
        WIFI_PASSWORD=\"${NODE_B_WIFI_PASSWORD}\"
    )
endif ()

if (NODE_B_MQTT_SERVER)
    target_compile_definitions(node_b PRIVATE
        MQTT_SERVER=\"${NODE_B_MQTT_SERVER}\"
    )
endif ()

if (NODE_B_MQTT_USERNAME)
    target_compile_definitions(node_b PRIVATE
        MQTT_USERNAME=\"${NODE_B_MQTT_USERNAME}\"
    )
endif ()

if (NODE_B_MQTT_PASSWORD)
    target_compile_definitions(node_b PRIVATE
        MQTT_PASSWORD=\"${NODE_B_MQTT_PASSWORD}\"
    )
endif ()

pico_enable_stdio_usb(node_b 1)
pico_enable_stdio_uart(node_b 0)
pico_add_extra_outputs(node_b)
