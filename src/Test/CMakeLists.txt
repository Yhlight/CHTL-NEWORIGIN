# Create a new directory for CHTL-specific tests
file(GLOB CHTL_TEST_SOURCES "CHTLTest/*.cpp")

add_executable(run_tests
    example.test.cpp
    UtilTest/StringUtil.test.cpp
    UtilTest/FileSystemUtil.test.cpp
    UtilTest/ZipUtil.test.cpp
    ${CHTL_TEST_SOURCES}
)

# Link the test executable against Google Test and the libraries under test.
target_link_libraries(run_tests
    PRIVATE
    gtest_main
    CHTL_Util
    CHTL_Compiler
)

# Include Google Test CMake utilities
include(GoogleTest)

# Automatically discover and add tests to the CTest runner
gtest_discover_tests(run_tests)
