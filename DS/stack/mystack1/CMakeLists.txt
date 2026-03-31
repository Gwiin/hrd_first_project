cmake_minimum_required(VERSION 3.10)
project(stack)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(stack main.c stack.c)