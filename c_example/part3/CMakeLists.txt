# sudo apt install build-essential
# sudo apt install gdb
# cmakelists.txt 를 다시 저장해야 한다.

cmake_minimum_required(VERSION 3.10)
project(jyhwang0631)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

file(GLOB EXECUTABLES "${CMAKE_CURRENT_LIST_DIR}/*.c")

foreach(EXECUTABLE ${EXECUTABLES})
    get_filename_component(EXE_NAME ${EXECUTABLE} NAME_WE)
    add_executable(${EXE_NAME} ${EXECUTABLE})
endforeach()