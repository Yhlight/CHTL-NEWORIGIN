#pragma once

#include <string>
#include <vector>
#include <ostream>

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    COLON, SEMICOLON,
    COMMA, DOT,

    // Operators
    EQUAL,
    PLUS, MINUS, STAR, SLASH,
    PERCENT, STAR_STAR,

    // Logical Operators
    AMPERSAND_AMPERSAND, PIPE_PIPE,

    // Comparison Operators
    GREATER, LESS,
    GREATER_EQUAL, LESS_EQUAL,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    USE,
    EXCEPT,
    FROM,
    AS,
    DELETE,
    INSERT,
    AFTER,
    BEFORE,
    REPLACE,
    AT_TOP,
    AT_BOTTOM,
    INHERIT,
    IF,
    ELSE,
    ELSE_IF,

    // Comments
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    GENERATOR_COMMENT,

    // Special
    CHTL_JS_SELECTOR,
    CHTL_JS_ARROW,

    // End of File
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    switch (type) {
        case TokenType::LEFT_BRACE: os << "LEFT_BRACE"; break;
        case TokenType::RIGHT_BRACE: os << "RIGHT_BRACE"; break;
        case TokenType::LEFT_PAREN: os << "LEFT_PAREN"; break;
        case TokenType::RIGHT_PAREN: os << "RIGHT_PAREN"; break;
        case TokenType::LEFT_BRACKET: os << "LEFT_BRACKET"; break;
        case TokenType::RIGHT_BRACKET: os << "RIGHT_BRACKET"; break;
        case TokenType::COLON: os << "COLON"; break;
        case TokenType::SEMICOLON: os << "SEMICOLON"; break;
        case TokenType::COMMA: os << "COMMA"; break;
        case TokenType::DOT: os << "DOT"; break;
        case TokenType::EQUAL: os << "EQUAL"; break;
        case TokenType::PLUS: os << "PLUS"; break;
        case TokenType::MINUS: os << "MINUS"; break;
        case TokenType::STAR: os << "STAR"; break;
        case TokenType::SLASH: os << "SLASH"; break;
        case TokenType::PERCENT: os << "PERCENT"; break;
        case TokenType::STAR_STAR: os << "STAR_STAR"; break;
        case TokenType::AMPERSAND_AMPERSAND: os << "AMPERSAND_AMPERSAND"; break;
        case TokenType::PIPE_PIPE: os << "PIPE_PIPE"; break;
        case TokenType::GREATER: os << "GREATER"; break;
        case TokenType::LESS: os << "LESS"; break;
        case TokenType::GREATER_EQUAL: os << "GREATER_EQUAL"; break;
        case TokenType::LESS_EQUAL: os << "LESS_EQUAL"; break;
        case TokenType::IDENTIFIER: os << "IDENTIFIER"; break;
        case TokenType::STRING: os << "STRING"; break;
        case TokenType::NUMBER: os << "NUMBER"; break;
        case TokenType::TEXT: os << "TEXT"; break;
        case TokenType::STYLE: os << "STYLE"; break;
        case TokenType::SCRIPT: os << "SCRIPT"; break;
        case TokenType::TEMPLATE: os << "TEMPLATE"; break;
        case TokenType::CUSTOM: os << "CUSTOM"; break;
        case TokenType::ORIGIN: os << "ORIGIN"; break;
        case TokenType::IMPORT: os << "IMPORT"; break;
        case TokenType::NAMESPACE: os << "NAMESPACE"; break;
        case TokenType::USE: os << "USE"; break;
        case TokenType::EXCEPT: os << "EXCEPT"; break;
        case TokenType::FROM: os << "FROM"; break;
        case TokenType::AS: os << "AS"; break;
        case TokenType::DELETE: os << "DELETE"; break;
        case TokenType::INSERT: os << "INSERT"; break;
        case TokenType::AFTER: os << "AFTER"; break;
        case TokenType::BEFORE: os << "BEFORE"; break;
        case TokenType::REPLACE: os << "REPLACE"; break;
        case TokenType::AT_TOP: os << "AT_TOP"; break;
        case TokenType::AT_BOTTOM: os << "AT_BOTTOM"; break;
        case TokenType::INHERIT: os << "INHERIT"; break;
        case TokenType::IF: os << "IF"; break;
        case TokenType::ELSE: os << "ELSE"; break;
        case TokenType::ELSE_IF: os << "ELSE_IF"; break;
        case TokenType::SINGLE_LINE_COMMENT: os << "SINGLE_LINE_COMMENT"; break;
        case TokenType::MULTI_LINE_COMMENT: os << "MULTI_LINE_COMMENT"; break;
        case TokenType::GENERATOR_COMMENT: os << "GENERATOR_COMMENT"; break;
        case TokenType::CHTL_JS_SELECTOR: os << "CHTL_JS_SELECTOR"; break;
        case TokenType::CHTL_JS_ARROW: os << "CHTL_JS_ARROW"; break;
        case TokenType::END_OF_FILE: os << "END_OF_FILE"; break;
        default: os << "UNKNOWN_TOKEN";
    }
    return os;
}