#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    From,
    As,
    Use,
    Html5,
    Except,
    If,
    Else,

    // Literals
    Identifier,
    String,
    Number,

    // Operators
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    LParen,
    RParen,
    At,
    Colon,
    Semicolon,
    Assign,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Power,
    Question,
    Comma,
    Dot,
    Ampersand,

    // Special
    Comment,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    size_t line = 0;
    size_t column = 0;

    bool operator==(const Token& other) const {
        return type == other.type && value == other.value && line == other.line && column == other.column;
    }
};

#endif //CHTL_TOKEN_H