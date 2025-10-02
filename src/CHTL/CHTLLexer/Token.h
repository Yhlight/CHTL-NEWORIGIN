#pragma once

#include <string>

enum class TokenType {
    // Placeholders for various token types
    IDENTIFIER,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,
    COLON,
    EQUALS,
    STRING_LITERAL,
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    GENERATOR_COMMENT,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};