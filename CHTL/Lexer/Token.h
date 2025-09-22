#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    COLON,          // :
    EQUAL,          // =
    SEMICOLON,      // ;
    DOT,            // .
    HASH,           // #
    AMPERSAND,      // &
    QUESTION,       // ?
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )

    // Operators
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    PERCENT,        // %
    STAR_STAR,      // **

    // Relational Operators
    GREATER,        // >
    LESS,           // <
    GREATER_EQUAL,  // >=
    LESS_EQUAL,     // <=
    EQUAL_EQUAL,    // ==
    BANG_EQUAL,     // !=

    // Logical Operators
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||

    // Literals
    IDENTIFIER,     // element names, attribute names, etc.
    STRING,         // "text" or 'text'
    NUMBER,         // 100, 50.5

    // Comments (might be skipped or handled specially)
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* ... */
    HASH_COMMENT,   // #

    // Keywords (although CHTL has few, 'text' could be one)
    TEXT_KEYWORD,

    // Other
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    std::string to_string() const {
        return "Line " + std::to_string(line) + ", Col " + std::to_string(column) +
               ": " + lexeme + " (Type: " + std::to_string(static_cast<int>(type)) + ")";
    }
};
