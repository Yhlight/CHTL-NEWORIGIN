cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Google Test Integration ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Set QUIET to ON to keep the build log clean
FetchContent_MakeAvailable(googletest)


# --- CHTL Library ---
# Define a library for the CHTL compiler components.
add_library(chtl_lib STATIC
    CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTLParser/CHTLParser.cpp
    CHTL/CHTLGenerator/CHTLGenerator.cpp
)

# Ensure that the library's include directories are available.
target_include_directories(chtl_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Main Executable ---
add_executable(chtl_compiler app/main.cpp)
target_link_libraries(chtl_compiler PRIVATE chtl_lib)


# --- Testing ---
# Enable CTest
enable_testing()

# Define the test executable
add_executable(run_tests
    Test/CHTLLexer_test.cpp
    Test/CHTLParser_test.cpp
    Test/CHTLIntegration_test.cpp
)

# Link the test executable with our library and Google Test
target_link_libraries(run_tests PRIVATE
    chtl_lib
    GTest::gtest_main
)

# Add the test to CTest for easy execution
add_test(NAME unit_tests COMMAND run_tests)
