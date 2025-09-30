# Test configuration
set(CMAKE_CXX_STANDARD 17)

# Find Catch2
find_path(CATCH2_INCLUDE_DIR catch.hpp PATHS ${CMAKE_SOURCE_DIR})
if(NOT CATCH2_INCLUDE_DIR)
    message(FATAL_ERROR "Catch2 not found. Please ensure catch.hpp is in the include path.")
endif()

# Include Catch2
include_directories(${CATCH2_INCLUDE_DIR})

# Test sources
set(TEST_SOURCES
    test_main.cpp
    test_lexer.cpp
)

# Create test executable
add_executable(chtl_tests ${TEST_SOURCES})

# Link with CHTL libraries
target_link_libraries(chtl_tests chtl_core miniz)

# Add tests
add_test(NAME LexerTests COMMAND chtl_tests "[lexer]")
add_test(NAME AllTests COMMAND chtl_tests)

# Set test properties
set_tests_properties(AllTests PROPERTIES LABELS "all")
set_tests_properties(LexerTests PROPERTIES LABELS "lexer")