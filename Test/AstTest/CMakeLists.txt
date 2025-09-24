# Create an executable for the AST tests.
add_executable(AstTests
    test_parser.cpp
)

# Link the test executable against the CHTL library (which contains the Parser)
# and the Util library.
target_link_libraries(AstTests PRIVATE
    CHTL
    Util
)

# Add this executable to the list of tests that can be run by CTest.
add_test(NAME ParserTest COMMAND AstTests)
