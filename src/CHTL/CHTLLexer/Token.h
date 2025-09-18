#pragma once

#include <string>
#include <vector>

namespace CHTL
{
    enum class TokenType
    {
        // Meta
        TOKEN_UNKNOWN,
        TOKEN_EOF,

        // Punctuation
        TOKEN_LEFT_BRACE,  // {
        TOKEN_RIGHT_BRACE, // }
        TOKEN_COLON,       // :
        TOKEN_EQUAL,       // =
        TOKEN_SEMICOLON,   // ;
        TOKEN_LEFT_PAREN,  // (
        TOKEN_RIGHT_PAREN, // )
        TOKEN_DOT,         // .

        // Operators
        TOKEN_PLUS,        // +
        TOKEN_MINUS,       // -
        TOKEN_STAR,        // *
        TOKEN_SLASH,       // /
        TOKEN_PERCENT,     // %
        TOKEN_STAR_STAR,   // **

        // Literals
        TOKEN_IDENTIFIER,       // div, id, my_class
        TOKEN_QUOTED_STRING,    // "hello", 'world'
        TOKEN_UNQUOTED_LITERAL, // some text

        // Comments
        TOKEN_COMMENT,           // //, /* */ (ignored by generator)
        TOKEN_GENERATOR_COMMENT, // # (processed by generator)
    };

    struct Token
    {
        TokenType type;
        std::string lexeme;
        int line = 0;
    };
}
