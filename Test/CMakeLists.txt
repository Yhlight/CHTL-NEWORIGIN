cmake_minimum_required(VERSION 3.10)

add_executable(run-tests
    TestMain.cpp
    LexerTest.cpp
    ParserTest.cpp
    IntegrationTest.cpp
    UseTest.cpp
    ExceptTest.cpp
    ConstraintsTest.cpp
    LocalStyleTest.cpp
    ConditionalRenderingTest.cpp
    ImgTagTest.cpp
    ConditionalPropertyTest.cpp
    IfParserTest.cpp
    DynamicConditionalRenderingTest.cpp
)

target_include_directories(run-tests PUBLIC
    ../third-party
    ../Util
    ../CHTL
)

target_link_libraries(run-tests PRIVATE
    chtl-lib
    chtl-util
)