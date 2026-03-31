cmake_minimum_required(VERSION 3.10)
project(twodouble)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(twodouble main.c twodouble.c)