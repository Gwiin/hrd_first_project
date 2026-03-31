cmake_minimum_required(VERSION 3.10)
project(scoreProcess)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(scoreProcess main.c scoreProcess.c)