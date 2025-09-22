#pragma once

#include <string>
#include <utility>

namespace CHTL {

enum class TokenType {
    // Special
    Eof,
    Illegal,

    // Literals
    Identifier, // div, id, width
    String,     // "hello", 'world', helloworld

    // Symbols
    LBrace,     // {
    RBrace,     // }
    Colon,      // :
    Semicolon,  // ;
    Equals,     // = (CE对等式)
};

struct Token {
    TokenType type;
    std::string literal;

    Token(TokenType type, std::string literal)
        : type(type), literal(std::move(literal)) {}
};

} // CHTL
