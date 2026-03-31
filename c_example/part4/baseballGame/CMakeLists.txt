cmake_minimum_required(VERSION 3.10)
project(baseballGame)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(baseballGame main.c baseballGame.c)