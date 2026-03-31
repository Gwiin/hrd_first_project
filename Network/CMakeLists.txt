cmake_minimum_required(VERSION 3.10)
project(mqttPub)

include_directories(${CMAKE_CURRENT_LIST_DIR})

find_library(PAHO_MQTT_LIB paho-mqtt3.c)

add_executable(mqttPub main.c mqttPub.c)
target_link_libraries(mqttPub &{PAHO_MQTT_LIB})
