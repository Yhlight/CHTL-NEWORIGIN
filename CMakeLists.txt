# CMakeLists.txt for CHTL Project

# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name
project(CHTL_Compiler CXX)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set sources for the compiler executable
set(COMPILER_SOURCES
    main.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
)

# Add the root directory to the include path so we can use #include "CHTL/..."
include_directories(.)

# Add the executable target
add_executable(chtl_compiler ${COMPILER_SOURCES})
