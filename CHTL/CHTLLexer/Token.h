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

    // Literals
    IDENTIFIER,         // e.g., div, my_variable
    STRING_LITERAL,     // "hello" or 'hello'
    UNQUOTED_LITERAL,   // hello

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
