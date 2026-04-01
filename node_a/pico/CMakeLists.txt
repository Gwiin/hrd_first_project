cmake_minimum_required(VERSION 3.13)

if (NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set.")
endif ()

if (NOT PICO_BOARD)
    set(PICO_BOARD pico2_w CACHE STRING "Target board" FORCE)
endif ()

option(PICO_NO_PICOTOOL "Skip picotool fetch during local builds" OFF)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(node_a C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(NODE_A_WIFI_SSID "$ENV{WIFI_SSID}")
set(NODE_A_WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")
set(NODE_A_MQTT_SERVER "$ENV{MQTT_SERVER}")
set(NODE_A_MQTT_USERNAME "$ENV{MQTT_USERNAME}")
set(NODE_A_MQTT_PASSWORD "$ENV{MQTT_PASSWORD}")

add_executable(node_a
    src/project0.c
)

target_include_directories(node_a PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(node_a PRIVATE
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
)

if (NODE_A_WIFI_SSID)
    target_compile_definitions(node_a PRIVATE
        WIFI_SSID=\"${NODE_A_WIFI_SSID}\"
    )
endif ()

if (NODE_A_WIFI_PASSWORD)
    target_compile_definitions(node_a PRIVATE
        WIFI_PASSWORD=\"${NODE_A_WIFI_PASSWORD}\"
    )
endif ()

if (NODE_A_MQTT_SERVER)
    target_compile_definitions(node_a PRIVATE
        MQTT_SERVER=\"${NODE_A_MQTT_SERVER}\"
    )
endif ()

if (NODE_A_MQTT_USERNAME)
    target_compile_definitions(node_a PRIVATE
        MQTT_USERNAME=\"${NODE_A_MQTT_USERNAME}\"
    )
endif ()

if (NODE_A_MQTT_PASSWORD)
    target_compile_definitions(node_a PRIVATE
        MQTT_PASSWORD=\"${NODE_A_MQTT_PASSWORD}\"
    )
endif ()

pico_enable_stdio_usb(node_a 1)
pico_enable_stdio_uart(node_a 0)
pico_add_extra_outputs(node_a)
