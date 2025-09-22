#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special
    ILLEGAL,
    END_OF_FILE,
    WHITESPACE,

    // Identifiers & Literals
    IDENTIFIER,         // e.g., div, myVar
    UNQUOTED_LITERAL,   // e.g., red, 100px
    STRING_LITERAL,     // e.g., "hello world"
    NUMBER_LITERAL,     // e.g., 100, 3.14

    // Punctuation
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    COLON,              // :
    SEMICOLON,          // ;
    EQUALS,             // =
    COMMA,              // ,
    DOT,                // .
    QUESTION_MARK,      // ?
    AMPERSAND,          // &

    // Operators
    PLUS,               // +
    MINUS,              // -
    ASTERISK,           // *
    SLASH,              // /
    PERCENT,            // %
    DOUBLE_ASTERISK,    // **

    // Single-character keywords
    HASH,               // # (Generator comment)
    AT,                 // @

    // Word Keywords
    TEXT,
    STYLE,
    SCRIPT,
    USE,
    INHERIT,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    FROM,
    AS,
    EXCEPT,
    CUSTOM,
    TEMPLATE,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    INFO,
    EXPORT,
    NAME,
    ORIGINTYPE,
    HTML,
    JAVASCRIPT,
    CHTL,
    CJMOD,
    CONFIG,
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int column;

    // Helper for debugging
    std::string ToString() const {
        return "Line " + std::to_string(line) + ":" + std::to_string(column) +
               " Type: " + std::to_string(static_cast<int>(type)) +
               " Literal: " + literal;
    }
};
