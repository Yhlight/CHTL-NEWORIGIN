cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
add_executable(chtl
    CHTL/CompilerDispatcher/main.cpp
    CHTL/CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTL/CHTLParser/Parser.cpp
    CHTL/CHTL/CHTLGenerator/Generator.cpp
)

# Add include directories for our project
# This will allow us to use #include <CHTL/Lexer.h> for example
include_directories(.)

# We will add more source files and libraries here as the project grows.
