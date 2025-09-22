#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,
    LEFT_PAREN, RIGHT_PAREN,
    SEMICOLON, COLON, EQUALS,
    DOT, HASH, AMPERSAND, AT,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT, STAR_STAR,

    // Literals.
    IDENTIFIER,
    NUMBER,
    STRING_LITERAL, // "..."
    SINGLE_QUOTED_LITERAL, // '...'
    UNQUOTED_LITERAL, // a_bare_word

    // Comments
    SINGLE_LINE_COMMENT, // // ...
    MULTI_LINE_COMMENT,  // /* ... */
    GENERATOR_COMMENT,   // # ...

    // Keywords
    KEYWORD_INHERIT,

    // End of file.
    END_OF_FILE,

    // Unknown token
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(std::move(lexeme)), line(line), column(column) {}
};
