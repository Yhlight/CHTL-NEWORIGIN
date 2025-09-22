cmake_minimum_required(VERSION 3.10)
project(CHTLCompiler C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the executable
add_executable(chtl_compiler
    # Main test driver
    Test/main.cpp

    # CHTL Components
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp

    # Third-party libs
    miniz.c
)

# Add include directories if needed, though our paths are relative
# target_include_directories(chtl_compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
