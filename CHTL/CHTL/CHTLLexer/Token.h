#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON,
    HASH, DOT, AMPERSAND,
    PLUS, MINUS, SLASH,
    ASTERISK, PERCENT,
    GREATER, LESS, QUESTION,
    COMMA, AT_SIGN, EQUALS,

    // Literals
    IDENTIFIER,
    UNQUOTED_LITERAL,
    STRING_LITERAL,
    NUMBER_LITERAL,

    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIG,
    KEYWORD_USE,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_INHERIT,

    // Special
    COMMENT,
    ILLEGAL,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

} // namespace CHTL
