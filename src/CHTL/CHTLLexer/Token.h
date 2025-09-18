#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    UNKNOWN,
    END_OF_FILE,

    // Literals
    IDENTIFIER,         // e.g., div, my-class, width
    UNQUOTED_LITERAL,   // e.g., red, HelloWorld
    STRING_LITERAL,     // e.g., "hello world", 'some text'
    NUMBER_LITERAL,     // e.g., 100, 3.14
    DIMENSION,          // e.g., 100px, 50%

    // Punctuation & Brackets
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
    QUESTION,           // ?
    AT_SYMBOL,          // @
    POUND,              // #

    // Operators
    PLUS,               // +
    MINUS,              // -
    STAR,               // *
    SLASH,              // /
    PERCENT,            // %
    POWER,              // **
    AMPERSAND,          // &
    LOGICAL_AND,        // &&
    LOGICAL_OR,         // ||

    // Comments
    LINE_COMMENT,       // // a comment
    BLOCK_COMMENT,      // /* a comment */
    GENERATOR_COMMENT,  // # a comment

    // Top-level Keywords (e.g. [Keyword])
    KEYWORD_CUSTOM,
    KEYWORD_TEMPLATE,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIGURATION,
    KEYWORD_INFO,
    KEYWORD_EXPORT,

    // Other Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
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
    KEYWORD_HTML5,
};

struct Token {
    TokenType type = TokenType::UNKNOWN;
    std::string lexeme;
    size_t line = 0;
    size_t column = 0;
};

} // namespace CHTL
