# Enable testing
enable_testing()

# Find all test source files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Add the test executable
add_executable(run_tests ${TEST_SOURCES})

# Link the test executable against the CHTL library and Catch2
target_link_libraries(run_tests PRIVATE chtl_lib)

# Add include directories for the test executable
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR})

# Add the test to CTest
add_test(NAME run_tests COMMAND run_tests)