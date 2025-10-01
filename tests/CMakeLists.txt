cmake_minimum_required(VERSION 3.16)

# Test configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Catch2
find_path(CATCH2_INCLUDE_DIR catch.hpp
    PATHS ${CMAKE_SOURCE_DIR}/third-party
    NO_DEFAULT_PATH
)

if(NOT CATCH2_INCLUDE_DIR)
    message(FATAL_ERROR "Catch2 not found in third-party directory")
endif()

# Include directories
include_directories(${CATCH2_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/CHTL/include)

# Test sources
set(TEST_SOURCES
    test_main.cpp
    test_CHTLContext.cpp
    test_CHTLNode.cpp
    test_CHTLState.cpp
    test_CHTLStrategy.cpp
)

# Create test executable
add_executable(chtl_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(chtl_tests chtl_core)

# Add tests
add_test(NAME CHTLContextTests COMMAND chtl_tests "[CHTLContext]")
add_test(NAME CHTLNodeTests COMMAND chtl_tests "[CHTLNode]")
add_test(NAME CHTLStateTests COMMAND chtl_tests "[CHTLState]")
add_test(NAME CHTLStrategyTests COMMAND chtl_tests "[CHTLStrategy]")

# Enable test discovery
set_tests_properties(CHTLContextTests PROPERTIES LABELS "unit;context")
set_tests_properties(CHTLNodeTests PROPERTIES LABELS "unit;node")
set_tests_properties(CHTLStateTests PROPERTIES LABELS "unit;state")
set_tests_properties(CHTLStrategyTests PROPERTIES LABELS "unit;strategy")