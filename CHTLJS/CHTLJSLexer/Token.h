#pragma once

#include <string>

namespace CHTLJS {

    enum class TokenType {
        TOKEN_IDENTIFIER,
        TOKEN_STRING_LITERAL,
        TOKEN_UNQUOTED_LITERAL,
        TOKEN_LBRACE,
        TOKEN_RBRACE,
        TOKEN_COLON,
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_DIVIDE,
        TOKEN_EOF,
        TOKEN_UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int column;
    };

}