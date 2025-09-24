# Test CMakeLists.txt
add_subdirectory(UtilTest)
add_subdirectory(TokenTest)
add_subdirectory(AstTest)

# State machine test
add_executable(StateMachineTest
    StateMachineTest.cpp
)

target_link_libraries(StateMachineTest PRIVATE
    Scanner
    Util
)

# CHTL Parser test
add_executable(CHTLParserTest
    CHTLParserTest.cpp
)

target_link_libraries(CHTLParserTest PRIVATE
    CHTLParser
    CHTLNode
    CHTLLexer
    Scanner
    Util
)

# Integration tests
add_executable(IntegrationTest
    IntegrationTest.cpp
)

target_link_libraries(IntegrationTest PRIVATE
    CHTL
    CHTL_JS
    Scanner
    CompilerDispatcher
    CodeMerger
    Util
    ThirdParty
)

# Register tests
add_test(NAME UtilTest COMMAND UtilTest)
add_test(NAME TokenTest COMMAND TokenTest)
add_test(NAME AstTest COMMAND AstTest)
add_test(NAME StateMachineTest COMMAND StateMachineTest)
add_test(NAME CHTLParserTest COMMAND CHTLParserTest)
add_test(NAME IntegrationTest COMMAND IntegrationTest)