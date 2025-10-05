cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG    release-1.12.1
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# CHTL Library
add_library(chtl_lib src/CHTL/CHTLLexer/CHTLLexer.cpp src/CHTL/CHTLParser/CHTLParser.cpp)

# Main Executable
add_executable(chtl src/main.cpp)
target_link_libraries(chtl PRIVATE chtl_lib)

# Tests
add_executable(LexerTest src/Test/TokenTest/LexerTest.cpp)
target_link_libraries(LexerTest PRIVATE chtl_lib gtest_main)

add_test(NAME LexerTest COMMAND LexerTest)

# Parser Tests
add_executable(ParserTest src/Test/ParserTest/ParserTest.cpp)
target_link_libraries(ParserTest PRIVATE chtl_lib gtest_main)
add_test(NAME ParserTest COMMAND ParserTest)