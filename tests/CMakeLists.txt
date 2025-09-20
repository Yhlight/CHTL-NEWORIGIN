cmake_minimum_required(VERSION 3.20)

# Find required packages
find_package(Threads REQUIRED)

# Test executable
add_executable(chtl_tests
    test_main.cpp
    test_lexer.cpp
    test_scanner.cpp
    test_parser.cpp
    test_generator.cpp
    test_merger.cpp
)

# Link libraries - simplified for now
target_link_libraries(chtl_tests 
    Threads::Threads
)

# Include directories
target_include_directories(chtl_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Compiler flags
target_compile_options(chtl_tests PRIVATE
    -Wall -Wextra -std=c++17
)

# Add tests to CTest
enable_testing()
add_test(NAME LexerTests COMMAND chtl_tests --gtest_filter=LexerTest.*)
add_test(NAME ScannerTests COMMAND chtl_tests --gtest_filter=ScannerTest.*)
add_test(NAME ParserTests COMMAND chtl_tests --gtest_filter=ParserTest.*)
add_test(NAME GeneratorTests COMMAND chtl_tests --gtest_filter=GeneratorTest.*)
add_test(NAME MergerTests COMMAND chtl_tests --gtest_filter=MergerTest.*)