cmake_minimum_required(VERSION 3.16)

# Create test executable
add_executable(chtl_tests
    test_lexer.cpp
    test_parser.cpp
    test_generator.cpp
    test_context.cpp
    test_state_machine.cpp
    test_strategy_pattern.cpp
)

# Include directories
target_include_directories(chtl_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

# Link libraries
target_link_libraries(chtl_tests PRIVATE chtl_lib)

# Add test to CTest
add_test(NAME chtl_tests COMMAND chtl_tests)