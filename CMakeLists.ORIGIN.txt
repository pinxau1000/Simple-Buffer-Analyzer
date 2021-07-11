# ONLY FOR GITHUB ACTIONS
cmake_minimum_required(VERSION 3.19)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

add_executable(main main.cpp)

find_package(PythonInterp REQUIRED)
find_package(PythonLibs  REQUIRED)

include_directories(include)
include_directories(${PYTHON_INCLUDE_DIRS})

target_link_libraries(main ${PYTHON_LIBRARIES})
