#include "CHTLLexer/Lexer.h"
#include "CHTLLexer/Token.h"
#include <iostream>
#include <vector>
#include <cassert>

// A simple utility to print tokens for debugging if a test fails.
void printTokens(const std::vector<CHTL::Token>& tokens) {
    for (const auto& token : tokens) {
        token.print();
    }
}

void testSimpleTokens() {
    std::cout << "Running test: testSimpleTokens..." << std::endl;
    std::string source = "div { id: box; }";
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.tokenizeAll();

    // Expected tokens: IDENTIFIER, LEFT_BRACE, IDENTIFIER, COLON, IDENTIFIER, SEMICOLON, RIGHT_BRACE, END_OF_FILE
    assert(tokens.size() == 8);

    assert(tokens[0].type == CHTL::TokenType::IDENTIFIER);
    assert(tokens[0].lexeme == "div");

    assert(tokens[1].type == CHTL::TokenType::LEFT_BRACE);
    assert(tokens[1].lexeme == "{");

    assert(tokens[2].type == CHTL::TokenType::IDENTIFIER);
    assert(tokens[2].lexeme == "id");

    assert(tokens[3].type == CHTL::TokenType::COLON);
    assert(tokens[3].lexeme == ":");

    assert(tokens[4].type == CHTL::TokenType::IDENTIFIER);
    assert(tokens[4].lexeme == "box");

    assert(tokens[5].type == CHTL::TokenType::SEMICOLON);
    assert(tokens[5].lexeme == ";");

    assert(tokens[6].type == CHTL::TokenType::RIGHT_BRACE);
    assert(tokens[6].lexeme == "}");

    assert(tokens[7].type == CHTL::TokenType::END_OF_FILE);

    std::cout << "PASSED!" << std::endl;
}

int main() {
    testSimpleTokens();
    // In the future, we can add more test cases here.
    std::cout << "All TokenTest tests passed!" << std::endl;
    return 0;
}
