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
