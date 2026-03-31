cmake_minimum_required(VERSION 3.10)
project(stack4)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(stack4 main.c stack4.c)