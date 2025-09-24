#include <iostream>
#include <vector>
#include <string>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLLexer/Token.h"

// Helper to convert TokenType to string for printing
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::KW_TEXT: return "KW_TEXT";
        case TokenType::KW_STYLE: return "KW_STYLE";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ILLEGAL: return "ILLEGAL";
        // Add other token types as needed for testing
        default: return "UNKNOWN";
    }
}

int main() {
    std::cout << "--- Lexer Test ---" << std::endl;
    std::string source = R"(
        text {
            "hello world";
        }
    )";

    std::cout << "Source:\n" << source << std::endl;

    CHTLLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "\nTokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "Type: " << tokenTypeToString(token.type)
                  << ",\tLexeme: '" << token.lexeme
                  << "'\tLine: " << token.line << std::endl;
    }

    return 0;
}
