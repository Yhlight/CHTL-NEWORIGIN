#include "../CHTL/CHTLLexer/Lexer.h"
#include <iostream>
#include <vector>
#include <cassert>

// A helper function to compare tokens and provide detailed output on failure.
void assertTokenEqual(const Token& actual, const Token& expected) {
    if (actual.type != expected.type) {
        std::cerr << "\n--- TOKEN MISMATCH ---" << std::endl;
        std::cerr << "Assertion Failed: Token type mismatch." << std::endl;
        std::cerr << "Expected Type: " << static_cast<int>(expected.type)
                  << ", Actual Type: " << static_cast<int>(actual.type) << std::endl;
        std::cerr << "Expected Literal: \"" << expected.literal << "\"" << std::endl;
        std::cerr << "Actual Literal:   \"" << actual.literal << "\"" << std::endl;
        std::cerr << "On line: " << actual.line << std::endl;
        std::cerr << "----------------------" << std::endl;
    }
    assert(actual.type == expected.type);

    if (actual.literal != expected.literal) {
        std::cerr << "\n--- TOKEN MISMATCH ---" << std::endl;
        std::cerr << "Assertion Failed: Token literal mismatch." << std::endl;
        std::cerr << "Expected: \"" << expected.literal << "\"" << std::endl;
        std::cerr << "Actual:   \"" << actual.literal << "\"" << std::endl;
        std::cerr << "On line: " << actual.line << std::endl;
        std::cerr << "----------------------" << std::endl;
    }
    assert(actual.literal == expected.literal);

    if (actual.line != expected.line) {
        std::cerr << "\n--- TOKEN MISMATCH ---" << std::endl;
        std::cerr << "Assertion Failed: Token line number mismatch." << std::endl;
        std::cerr << "Expected: " << expected.line << std::endl;
        std::cerr << "Actual:   " << actual.line << std::endl;
        std::cerr << "For literal: \"" << actual.literal << "\"" << std::endl;
        std::cerr << "----------------------" << std::endl;
    }
    assert(actual.line == expected.line);
}

void testBasicSyntax() {
    std::cout << "  Testing Basic CHTL Syntax..." << std::endl;

    std::string input = R"(
html {
    body {
        div {
            text {
                "Hello, World!"
            }
        }
    }
}
)";

    std::vector<Token> expectedTokens = {
        {TokenType::IDENTIFIER, "html", 2},
        {TokenType::LEFT_BRACE, "{", 2},
        {TokenType::IDENTIFIER, "body", 3},
        {TokenType::LEFT_BRACE, "{", 3},
        {TokenType::IDENTIFIER, "div", 4},
        {TokenType::LEFT_BRACE, "{", 4},
        {TokenType::TEXT, "text", 5},
        {TokenType::LEFT_BRACE, "{", 5},
        {TokenType::STRING, "Hello, World!", 6},
        {TokenType::RIGHT_BRACE, "}", 7},
        {TokenType::RIGHT_BRACE, "}", 8},
        {TokenType::RIGHT_BRACE, "}", 9},
        {TokenType::RIGHT_BRACE, "}", 10},
        {TokenType::END_OF_FILE, "", 11}
    };

    Lexer l(input);

    for (const auto& expected : expectedTokens) {
        Token tok = l.nextToken();
        assertTokenEqual(tok, expected);
    }

    std::cout << "  ...Passed" << std::endl;
}

void testAttributeTokenization() {
    std::cout << "  Testing Attribute Tokenization..." << std::endl;

    std::string input = R"(
div {
    id = "main-content";
    class: "container";
}
)";

    std::vector<Token> expectedTokens = {
        {TokenType::IDENTIFIER, "div", 2},
        {TokenType::LEFT_BRACE, "{", 2},
        {TokenType::IDENTIFIER, "id", 3},
        {TokenType::EQUALS, "=", 3},
        {TokenType::STRING, "main-content", 3},
        {TokenType::SEMICOLON, ";", 3},
        {TokenType::IDENTIFIER, "class", 4},
        {TokenType::COLON, ":", 4},
        {TokenType::STRING, "container", 4},
        {TokenType::SEMICOLON, ";", 4},
        {TokenType::RIGHT_BRACE, "}", 5},
        {TokenType::END_OF_FILE, "", 6}
    };

    Lexer l(input);

    for (const auto& expected : expectedTokens) {
        Token tok = l.nextToken();
        assertTokenEqual(tok, expected);
    }

    std::cout << "  ...Passed" << std::endl;
}

void RunLexerTests() {
    std::cout << "--- Running Lexer Tests ---" << std::endl;
    testBasicSyntax();
    testAttributeTokenization();
    std::cout << "--------------------------" << std::endl;
}
