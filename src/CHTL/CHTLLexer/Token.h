#pragma once

#include <string>
#include <vector>

namespace CHTL {

    enum class TokenType {
        // Literals
        IDENTIFIER,
        STRING_LITERAL,
        NUMERIC_LITERAL,

        // Symbols
        LEFT_BRACE,   // {
        RIGHT_BRACE,  // }
        LEFT_PAREN,   // (
        RIGHT_PAREN,  // )
        COLON,        // :
        SEMICOLON,    // ;
        ASSIGN,       // =
        COMMA,        // ,

        // Comments
        GENERATOR_COMMENT,

        // Misc
        UNKNOWN,
        END_OF_FILE
    };

    struct Token {
        TokenType type;
        std::string value;
        int line;
        int column;
    };

} // namespace CHTL
