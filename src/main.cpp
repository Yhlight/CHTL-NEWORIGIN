#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"

// Function to convert TokenType to string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::TEXT_KEYWORD: return "TEXT_KEYWORD";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNDEFINED";
    }
}

int main() {
    std::ifstream file("sample.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open sample.chtl" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    CHTLLexer lexer;
    std::vector<Token> tokens = lexer.tokenize(content);

    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ", Value: '" << token.value << "'" << std::endl;
    }

    return 0;
}