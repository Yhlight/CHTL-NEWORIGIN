#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LBRACE, RBRACE, // { }
    COLON, SEMICOLON, // : ;
    COMMA, // ,
    DOT, // .
    HASH, // #

    // Literals
    IDENTIFIER, // div, text, my_variable
    STRING,     // "hello world"
    NUMBER,     // 100, 3.14

    // Comments
    HASH_COMMENT, // # a comment

    // Keywords (will be handled as identifiers for now)
    // TEXT,

    // Special tokens
    TOKEN_EOF,
    TOKEN_ERROR
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

// Helper function for debugging
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::HASH_COMMENT: return "HASH_COMMENT";
        case TokenType::TOKEN_EOF: return "EOF";
        case TokenType::TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

#endif // CHTL_TOKEN_H
