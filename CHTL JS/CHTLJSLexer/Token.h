#pragma once

#include <string>

namespace CHTLJS {

enum class TokenType {
    TOKEN_DOUBLE_LBRACE, // {{
    TOKEN_DOUBLE_RBRACE, // }}
    TOKEN_IDENTIFIER,
    TOKEN_DOT,
    TOKEN_HASH,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_NUMERIC_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
};

}