cmake_minimum_required(VERSION 3.13)

if (NOT DEFINED ENV{PICO_SDK_PATH})
    message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set.")
endif ()

if (NOT PICO_BOARD)
    set(PICO_BOARD pico2_w CACHE STRING "Target board" FORCE)
endif ()

option(PICO_NO_PICOTOOL "Skip picotool fetch during local builds" OFF)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(node_c C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(NODE_C_WIFI_SSID "$ENV{WIFI_SSID}")
set(NODE_C_WIFI_PASSWORD "$ENV{WIFI_PASSWORD}")
set(NODE_C_MQTT_SERVER "$ENV{MQTT_SERVER}")
set(NODE_C_MQTT_USERNAME "$ENV{MQTT_USERNAME}")
set(NODE_C_MQTT_PASSWORD "$ENV{MQTT_PASSWORD}")

add_executable(node_c
    src/main.c
    src/node_c_controller.c
    src/node_c_network.c
)

target_include_directories(node_c PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(node_c
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mqtt
)

if (NODE_C_WIFI_SSID)
    target_compile_definitions(node_c PRIVATE
        WIFI_SSID=\"${NODE_C_WIFI_SSID}\"
    )
endif ()

if (NODE_C_WIFI_PASSWORD)
    target_compile_definitions(node_c PRIVATE
        WIFI_PASSWORD=\"${NODE_C_WIFI_PASSWORD}\"
    )
endif ()

if (NODE_C_MQTT_SERVER)
    target_compile_definitions(node_c PRIVATE
        MQTT_SERVER=\"${NODE_C_MQTT_SERVER}\"
    )
endif ()

if (NODE_C_MQTT_USERNAME)
    target_compile_definitions(node_c PRIVATE
        MQTT_USERNAME=\"${NODE_C_MQTT_USERNAME}\"
    )
endif ()

if (NODE_C_MQTT_PASSWORD)
    target_compile_definitions(node_c PRIVATE
        MQTT_PASSWORD=\"${NODE_C_MQTT_PASSWORD}\"
    )
endif ()

pico_enable_stdio_usb(node_c 1)
pico_enable_stdio_uart(node_c 0)

pico_add_extra_outputs(node_c)
