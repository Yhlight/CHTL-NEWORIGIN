# Automatically find all test source files
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

# Create the test executable
add_executable(CHTL_Tests ${TEST_SOURCES})

# Link the test executable against the CHTL library and Catch2
target_link_libraries(CHTL_Tests PRIVATE CHTL)

# Add test to CTest
add_test(NAME CHTL_Tests COMMAND CHTL_Tests)