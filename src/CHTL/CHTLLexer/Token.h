#pragma once

#include <string>

enum class TokenType {
    // Placeholders for various token types
    IDENTIFIER,
    CLASS_SELECTOR,
    ID_SELECTOR,
    CONTEXT_SELECTOR,
    PROPERTY_REFERENCE,
    NUMBER,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_PAREN,
    RIGHT_PAREN,
    DOT,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    POWER,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LOGICAL_AND,
    LOGICAL_OR,
    QUESTION_MARK,
    SEMICOLON,
    COLON,
    COMMA,
    EQUALS,
    STRING_LITERAL,
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_CONDITION,
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