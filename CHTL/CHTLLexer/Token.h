#pragma once

#include <string>
#include <vector>

namespace CHTL {

// Enum for all possible token types in the CHTL language.
enum class TokenType {
    // Special Tokens
    UNKNOWN,
    END_OF_FILE,

    // Literals
    IDENTIFIER,         // e.g., div, myVar, .class
    UNQUOTED_LITERAL,   // e.g., red, 100px, solid
    STRING_LITERAL,     // "hello" or 'hello'
    NUMBER_LITERAL,     // 123 or 45.67

    // Punctuation & Symbols
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    COLON,              // :
    EQUALS,             // =
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    AT,                 // @
    HASH,               // #
    DOLLAR,             // $

    // Comments
    LINE_COMMENT,       // // a comment
    BLOCK_COMMENT,      // /* a comment */
    GENERATOR_COMMENT,  // # a comment

    // Operators
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    PERCENT,            // %
    DOUBLE_STAR,        // **
    QUESTION_MARK,      // ?
    AMPERSAND,          // &
    DOUBLE_AMPERSAND,   // &&
    PIPE,               // |
    DOUBLE_PIPE,        // ||
    GREATER_THAN,       // >
    LESS_THAN,          // <
    ARROW,              // ->

    // Keywords (will be expanded)
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_FROM,
    KEYWORD_AS
};

// Represents a single token extracted by the lexer.
struct Token {
    TokenType type = TokenType::UNKNOWN;
    std::string value;
    int line = 0;
    int column = 0;
};

} // namespace CHTL
