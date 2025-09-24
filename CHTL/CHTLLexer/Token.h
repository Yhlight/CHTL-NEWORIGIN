#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    LEFT_PAREN, RIGHT_PAREN,
    AT,
    HASH,
    COLON, SEMICOLON,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,
    COMMENT,

    // Operators
    PLUS, MINUS, STAR, SLASH,
    GREATER,
    QUESTION,

    // End of file
    END_OF_FILE,

    // Unknown
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    // int line; // For future error reporting
};

// Helper function for debugging
std::string TokenTypeToString(TokenType type);

} // namespace CHTL

#endif // CHTL_TOKEN_H
