cmake_minimum_required(VERSION 3.19)
project(main)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS -static)

# add_library(
#         MY_LIB
#         STATIC
#         include/Analyzer.cpp
#         include/Bitstream.cpp
#         include/Buffer.cpp
#         include/Frame.cpp
#         include/Analyzer.hpp
#         include/Bitstream.hpp
#         include/Buffer.hpp
#         include/Frame.hpp
#         include/Definitions.hpp
# )

file(GLOB APP_SOURCES *_buffer_size.cpp)
foreach(source_file ${APP_SOURCES})
    get_filename_component(exec_name ${source_file} NAME_WE)
    add_executable(
            ${exec_name}
            ${source_file}
            include/Analyzer.cpp
            include/Bitstream.cpp
            include/Buffer.cpp
            include/Frame.cpp
            include/Definitions.cpp
            include/Analyzer.hpp
            include/Bitstream.hpp
            include/Buffer.hpp
            include/Frame.hpp
            include/Definitions.hpp)
    # target_link_libraries(${exec_name} PRIVATE MY_LIB)
    target_include_directories(${exec_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
endforeach(source_file ${APP_SOURCES})
