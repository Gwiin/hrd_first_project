cmake_minimum_required(VERSION 3.10)
project(stack3)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(stack3 main.c stack3.c)