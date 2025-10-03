add_executable(runTests
    LexerTest.cpp
    ParserTest.cpp
    AttributeAutomatorTest.cpp
    CssGeneratorTest.cpp
    ExpressionParserTest.cpp
    ExpressionEvaluatorTest.cpp
    IfBehaviorTest.cpp
)

target_link_libraries(runTests
    PRIVATE
    CHTL
    ASTPrinter
    AttributeAutomator
    CssGenerator
    ExpressionParser
    ExpressionEvaluator
    StyleEvaluator
    gtest_main
)

target_include_directories(runTests PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(runTests)