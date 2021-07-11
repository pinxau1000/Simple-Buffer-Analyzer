# USE THIS FOR LOCAL COMPILATION
cmake_minimum_required(VERSION 3.19)
project(main)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(main main.cpp)

target_include_directories(main PRIVATE include)