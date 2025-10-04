add_executable(runTests
    LexerTest.cpp
    ParserTest.cpp
    AttributeAutomatorTest.cpp
    CssGeneratorTest.cpp
    ExpressionParserTest.cpp
    ExpressionEvaluatorTest.cpp
    ImportResolverTest.cpp
)

target_link_libraries(runTests
    PRIVATE
    CHTL
    ASTPrinter
    AttributeAutomator
    CssGenerator
    ExpressionParser
    ExpressionEvaluator
    ImportResolver
    gtest_main
)

target_include_directories(runTests PRIVATE ${CMAKE_SOURCE_DIR}/src)

include(GoogleTest)
gtest_discover_tests(runTests)