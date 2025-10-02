cmake_minimum_required(VERSION 3.10)

add_executable(run-tests
    LexerTest.cpp
    ParserTest.cpp
    IntegrationTest.cpp
    UseTest.cpp
    ExceptTest.cpp
    ConstraintsTest.cpp
    LocalStyleTest.cpp
    ConditionTest.cpp
)

target_include_directories(run-tests PUBLIC
    ../third-party
)

target_link_libraries(run-tests PRIVATE
    chtl-lib
)