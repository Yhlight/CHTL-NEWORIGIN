add_executable(runTests
    LexerTest.cpp
    ParserTest.cpp
)

target_link_libraries(runTests
    PRIVATE
    CHTL
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(runTests)