cmake_minimum_required(VERSION 3.10)
project(bowling)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(bowling2 main.c bowling.c)