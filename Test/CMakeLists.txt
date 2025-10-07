# Add the test executable
add_executable(CHTL_Tests
    main.cpp
)

# Link the test executable against the CHTL library
target_link_libraries(CHTL_Tests PRIVATE CHTL)

# Add the test to CTest
add_test(NAME CHTL_Tests COMMAND CHTL_Tests)