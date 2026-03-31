cmake_minimum_required(VERSION 3.10)
project(stack2)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(stack2 main.c stack2.c)