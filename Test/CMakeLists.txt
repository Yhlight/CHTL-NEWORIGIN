cmake_minimum_required(VERSION 3.10)
project(CHTL_Tests)

add_executable(chlt-test CHTL/LexerTest.cpp)

# Link against the CHTL library
target_link_libraries(chlt-test PRIVATE chtl_lib)

# Include the catch.hpp header
target_include_directories(chlt-test PRIVATE ${CMAKE_SOURCE_DIR})

add_test(NAME chtl-test COMMAND chlt-test)