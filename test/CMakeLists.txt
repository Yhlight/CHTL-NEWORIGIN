# Test Suite using Catch2

set(TEST_SOURCES
    test_main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_generator.cpp
)

add_executable(chtl_tests ${TEST_SOURCES})

target_link_libraries(chtl_tests 
    PRIVATE 
        chtl_lib
        chtljs_lib
        shared_core
        util_lib
)

target_include_directories(chtl_tests 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/third-party
)

# Register tests with CTest
add_test(NAME CHTLTests COMMAND chtl_tests)
