#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unknown,

    // Literals
    Identifier,
    StringLiteral,

    // Punctuation
    LeftBrace,  // {
    RightBrace, // }
};

struct Token {
    TokenType type;
    std::string value;
    // Basic position info for now
    int line;
    int column;
};