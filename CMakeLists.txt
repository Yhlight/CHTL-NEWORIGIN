cmake_minimum_required(VERSION 3.15)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler options
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third-party)

# Add subdirectories
add_subdirectory(src/CHTL)
add_subdirectory(src/CHTLJS)
add_subdirectory(src/SharedCore)
add_subdirectory(src/Util)

# Testing
enable_testing()
add_subdirectory(test)

# Main executable
add_executable(chtl-compiler src/main.cpp)
target_link_libraries(chtl-compiler 
    PRIVATE 
    chtl_lib
    chtljs_lib
    shared_core
    util_lib
)

# Installation
install(TARGETS chtl-compiler DESTINATION bin)
install(DIRECTORY module/ DESTINATION share/chtl/module OPTIONAL)
