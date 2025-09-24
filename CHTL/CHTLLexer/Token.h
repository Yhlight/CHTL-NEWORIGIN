#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace CHTL {

// Enum for all possible token types in the CHTL language.
enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    COLON, SEMICOLON, COMMA,     // : ; ,
    DOT, AT, HASH,               // . @ #

    // Operators
    PLUS, MINUS, STAR, SLASH,    // + - * /
    PERCENT,                     // %
    QUESTION,                    // ?
    EQUAL,                       // =
    GREATER, LESS,                // > <

    // Multi-character operators
    POWER,          // **
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    GREATER_EQUAL,  // >=
    LESS_EQUAL,     // <=
    AND, OR,        // &&, ||
    ARROW,          // ->
    EVENT_ARROW,    // &->

    // Literals
    IDENTIFIER,
    STRING_LITERAL,      // "text" or 'text'
    NUMBER_LITERAL,
    UNQUOTED_LITERAL,    // text without quotes

    // Special Keywords (Bracketed)
    // These will be identified by their content, but we need a generic type.
    BRACKETED_KEYWORD, // e.g., [Template], [Custom], etc.

    // At-Keywords
    AT_KEYWORD, // e.g., @Style, @Element, @Html

    // Identifier Keywords
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_ATTOP,
    KEYWORD_ATBOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_USE,
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_IF,
    KEYWORD_ELSE,

    // Comments
    LINE_COMMENT,  // // ...
    BLOCK_COMMENT, // /* ... */
    GEN_COMMENT,   // # ...

    // End of File
    END_OF_FILE,

    // Error / Unknown
    UNKNOWN
};

// Represents a single token produced by the Lexer.
struct Token {
    TokenType type;
    std::string lexeme; // The actual text of the token
    int line;           // Line number where the token starts
    int column;         // Column number where the token starts

    // Helper for debugging
    void print() const {
        std::cout << "Token(" << static_cast<int>(type) << ", '" << lexeme << "', L" << line << ":C" << column << ")" << std::endl;
    }
};

} // namespace CHTL
