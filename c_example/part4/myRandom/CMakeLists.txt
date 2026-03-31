cmake_minimum_required(VERSION 3.10)
project(myRandom)

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_executable(myRandom main.c myRandom.c)