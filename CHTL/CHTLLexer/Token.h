#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special
    TOKEN_EOF,
    TOKEN_ERROR,

    // Literals
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_UNQUOTED_LITERAL,

    // Keywords (will be expanded later)
    TOKEN_TEXT,
    TOKEN_STYLE,

    // Symbols
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACKET,     // [
    TOKEN_RBRACKET,     // ]
    TOKEN_COLON,        // :
    TOKEN_SEMICOLON,    // ;
    TOKEN_EQUALS,       // =
    TOKEN_COMMA,        // ,
    TOKEN_DOT,          // .

    // Operators
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_PERCENT,      // %
    TOKEN_DOUBLE_STAR,  // **
    TOKEN_QUESTION,     // ?
    TOKEN_AMPERSAND,    // &

    // Comments
    TOKEN_COMMENT,          // // or /* */
    TOKEN_GENERATOR_COMMENT // #
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};
