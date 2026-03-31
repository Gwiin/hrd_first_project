cmake_minimum_required(VERSION 3.10)
project(date)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(date main.c date.c)