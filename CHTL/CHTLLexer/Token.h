#pragma once

#include <string>
#include <utility>

namespace CHTL {

enum class TokenType {
    // Special
    Eof,
    Illegal,

    // Literals
    Identifier, // div, id, width, 100px, background-color
    String,     // "hello", 'world'

    // Comments
    GeneratorComment, // # a comment

    // Symbols
    LBrace,     // {
    RBrace,     // }
    Colon,      // :
    Semicolon,  // ;
    Equals,     // =
};

struct Token {
    TokenType type;
    std::string literal;

    Token(TokenType type, std::string literal)
        : type(type), literal(std::move(literal)) {}
};

} // CHTL
