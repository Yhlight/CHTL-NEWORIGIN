# Add the subdirectory for the compiler library
add_subdirectory(CHTL)

# Define the main executable
add_executable(CHTLCompiler Test/E2ETest.cpp)

# Link the executable against the compiler library
target_link_libraries(CHTLCompiler PRIVATE chtl_lib)

# Add a test case for CTest
add_test(
    NAME RunComprehensiveTest
    COMMAND ${CMAKE_BINARY_DIR}/CHTL/CHTLCompiler ${CMAKE_SOURCE_DIR}/main.chtl
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
