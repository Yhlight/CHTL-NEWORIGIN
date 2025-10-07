#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Brackets and Braces
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]

    // Punctuation
    COLON, SEMICOLON, COMMA, DOT, QUESTION, AT, HASH, DOLLAR,

    // Operators
    EQUAL,
    PLUS, MINUS, STAR, SLASH, PERCENT,
    STAR_STAR, // ** (Power)

    // Logical Operators
    AMPERSAND, AMPERSAND_AMPERSAND, // & &&
    PIPE, PIPE_PIPE,             // | ||

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,
    UNQUOTED_LITERAL,

    // Keywords
    TEXT, STYLE, SCRIPT,
    IF, ELSE,
    INSERT, DELETE,
    FROM, AS, USE, EXCEPT,
    INHERIT, AFTER, BEFORE, REPLACE,
    KEYWORD_AT_TOP,      // "at top"
    KEYWORD_AT_BOTTOM,   // "at bottom"
    HTML5,

    // Special Keywords in Brackets
    KEYWORD_CUSTOM,        // [Custom]
    KEYWORD_TEMPLATE,      // [Template]
    KEYWORD_ORIGIN,        // [Origin]
    KEYWORD_IMPORT,        // [Import]
    KEYWORD_NAMESPACE,     // [Namespace]
    KEYWORD_CONFIGURATION, // [Configuration]
    KEYWORD_INFO,          // [Info]
    KEYWORD_EXPORT,        // [Export]

    // CHTL JS Specific
    LEFT_DOUBLE_BRACE, RIGHT_DOUBLE_BRACE, // {{ }}
    ARROW,                                 // ->
    AMPERSAND_ARROW,                       // &->

    // Miscellaneous
    UNRECOGNIZED,
    END_OF_FILE,
};

struct Token {
    TokenType type;
    std::string lexeme;
    // Using std::variant for literal value, can be expanded later if needed.
    std::variant<std::monostate, std::string, double> literal;
    int line;
    int column;
};

} // namespace CHTL