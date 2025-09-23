#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Meta
    UNKNOWN,
    END_OF_FILE,

    // Structural Symbols
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    COLON,          // :
    SEMICOLON,      // ;
    COMMA,          // ,
    AT,             // @
    DOT,            // .
    HASH,           // #
    AMPERSAND,      // &

    // Operators
    EQUAL,          // =
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    PERCENT,        // %
    DOUBLE_STAR,    // **

    // Logical & Conditional Operators
    GREATER,        // >
    LESS,           // <
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    QUESTION_MARK,  // ?

    // Literals
    IDENTIFIER,     // e.g., div, my_variable
    STRING,         // "hello", 'world', or unquoted_literal
    NUMBER,         // e.g., 100, 3.14

    // Comments
    COMMENT,        // // or /* */ style comments
    HASH_COMMENT,   // # style comments

    // Keywords - will be expanded later
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_ELEMENT,
    KEYWORD_VAR,
    KEYWORD_IMPORT,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_ORIGIN,
    KEYWORD_NAMESPACE,
    KEYWORD_USE,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_HTML5,
    KEYWORD_HTML,
    KEYWORD_JAVASCRIPT,
    KEYWORD_CHTL,
    KEYWORD_CONFIGURATION,
    KEYWORD_NAME,
    KEYWORD_EXCEPT
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
};
