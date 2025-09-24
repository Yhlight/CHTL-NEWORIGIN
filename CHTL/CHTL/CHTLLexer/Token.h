#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

enum class TokenType {
    // Meta
    ILLEGAL,
    END_OF_FILE,

    // Literals
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,

    // Punctuation
    LEFT_BRACE,   // {
    RIGHT_BRACE,  // }
    LEFT_PAREN,   // (
    RIGHT_PAREN,  // )
    LEFT_BRACKET, // [
    RIGHT_BRACKET,// ]
    SEMICOLON,    // ;
    COLON,        // :
    EQUALS,       // =
    COMMA,        // ,
    DOT,          // .
    AMPERSAND,    // &
    HASHTAG,      // #

    // Operators
    PLUS,         // +
    MINUS,        // -
    ASTERISK,     // *
    SLASH,        // /
    PERCENT,      // %

    // Comments
    COMMENT, // Generic, might not be used if we skip comments in lexer
    GENERATOR_COMMENT, // For # comments

    // Keywords - will be identified from IDENTIFIERs
    KW_TEXT,
    KW_STYLE,
    KW_SCRIPT,
    KW_TEMPLATE,
    KW_CUSTOM,
    KW_ORIGIN,
    KW_IMPORT,
    KW_NAMESPACE,
    KW_CONFIGURATION
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line = 0;
};

#endif // CHTL_TOKEN_H
