# Create an executable for the token tests.
add_executable(TokenTests
    test_lexer.cpp
)

# Link the test executable against the CHTL library, which contains the Lexer.
# We also link against Util because the CHTL library might depend on it,
# and it's good practice to be explicit.
target_link_libraries(TokenTests PRIVATE
    CHTL
    Util
)

# Add this executable to the list of tests that can be run by CTest.
# This allows us to run 'make test' or 'ctest' to execute all tests.
add_test(NAME LexerTest COMMAND TokenTests)
