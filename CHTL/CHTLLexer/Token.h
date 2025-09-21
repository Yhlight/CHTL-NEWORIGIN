#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens.
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON,
    DOT, COMMA,
    HASH, // #
    AMPERSAND, // &

    // Operators
    EQUAL,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    STAR_STAR, // for power

    // Logical Operators
    AMPERSAND_AMPERSAND, // &&
    PIPE_PIPE, // ||

    // Comparison Operators
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    EQUAL_EQUAL, BANG_EQUAL, // ==, !=

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,
    UNQUOTED_LITERAL,

    // Keywords
    KEYWORD_TEXT, KEYWORD_STYLE, KEYWORD_SCRIPT,
    KEYWORD_INHERIT, KEYWORD_DELETE, KEYWORD_INSERT,
    KEYWORD_AFTER, KEYWORD_BEFORE, KEYWORD_REPLACE, KEYWORD_AT, KEYWORD_TOP, KEYWORD_BOTTOM,
    KEYWORD_FROM, KEYWORD_AS, KEYWORD_EXCEPT, KEYWORD_USE, KEYWORD_HTML5,

    // Comments
    LINE_COMMENT, // //
    BLOCK_COMMENT, // /* */
    GENERATOR_COMMENT, // #

    // Special Keywords
    KEYWORD_TEMPLATE, // [Template]
    KEYWORD_CUSTOM,   // [Custom]
    KEYWORD_ORIGIN,   // [Origin]
    KEYWORD_IMPORT,   // [Import]
    KEYWORD_NAMESPACE,// [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]

    // @-rules
    AT_STYLE,
    AT_ELEMENT,
    AT_VAR,
    AT_HTML,
    AT_CSS,
    AT_JAVASCRIPT,
    AT_CHTL,
    AT_CJMOD,
    AT_CONFIG,

    // Other
    TERNARY_QUESTION, // ?

    // End of file.
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int col;
};

} // namespace CHTL
