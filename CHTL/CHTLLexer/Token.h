#pragma once

#include <string>
#include <vector>

namespace CHTL {

// Defines the different types of tokens that the lexer can produce.
enum class TokenType {
    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,

    // Symbols & Operators
    LEFT_BRACE,    // {
    RIGHT_BRACE,   // }
    COLON,         // :
    EQUALS,        // =
    SEMICOLON,     // ;
    LEFT_PAREN,    // (
    RIGHT_PAREN,   // )
    PLUS,          // +
    MINUS,         // -
    STAR,          // *
    SLASH,         // /
    PERCENT,       // %
    STAR_STAR,     // **
    GREATER,       // >
    LESS,          // <
    QUESTION,      // ?
    AMPERSAND_AMPERSAND, // &&
    PIPE_PIPE,     // ||

    // Literals
    IDENTIFIER,         // e.g., div, my_variable
    STRING_LITERAL,     // "hello" or 'hello'
    NUMBER_LITERAL,     // 100, 3.14
    UNQUOTED_LITERAL,   // hello, red, 100px (for now)

    // Comments
    SINGLE_LINE_COMMENT, // // a comment
    MULTI_LINE_COMMENT,  // /* a comment */
    GENERATOR_COMMENT,   // # a comment

    // Meta
    END_OF_FILE,
    UNKNOWN
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line = 0;
    int column = 0;
};

} // namespace CHTL
