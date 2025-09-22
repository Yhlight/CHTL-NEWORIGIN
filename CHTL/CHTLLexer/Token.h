#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LBRACE, RBRACE, COLON, SEMICOLON, HASH,
    LBRACKET, RBRACKET, AT,
    PLUS, MINUS, STAR, SLASH,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    TEXT,
    STYLE,
    TEMPLATE,
    CUSTOM,
    IMPORT,
    FROM,
    CHTL,

    // Other
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    // Default constructor
    Token() : type(TokenType::UNKNOWN), line(0), column(0) {}

    // Parameterized constructor
    Token(TokenType t, std::string v, int l, int c) : type(t), value(std::move(v)), line(l), column(c) {}
};

// Helper function for debugging
std::string token_type_to_string(TokenType type);
