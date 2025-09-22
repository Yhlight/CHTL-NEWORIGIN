cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an include directory for our project
include_directories(.)

# Add our source files
add_executable(chtl_compiler
    main.cpp
    CHTL/Lexer/Lexer.cpp
    CHTL/Parser/Parser.cpp
    CHTL/Generator/Generator.cpp
)
