#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special tokens
    ILLEGAL,
    END_OF_FILE,

    // Single-character tokens
    LEFT_BRACE,
    RIGHT_BRACE,

    // Literals
    IDENTIFIER,
    STRING,

    // Keywords
    TEXT,

    // Punctuators
    COLON,
    EQUALS,
    SEMICOLON
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
};
