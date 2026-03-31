cmake_minimum_required(VERSION 3.10)
project(queue)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(queue3 main.c queue3.c)