#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // Basic
    UNKNOWN,
    EndOfFile,
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER,
    DIMENSION,
    COMMENT,

    // Keywords
    TEXT_KEYWORD,
    STYLE_KEYWORD,
    SCRIPT_KEYWORD,

    // Punctuation
    L_BRACE,
    R_BRACE,
    L_PAREN,
    R_PAREN,
    L_BRACKET,
    R_BRACKET,
    AT,
    COLON,
    EQUAL,
    DOT,
    HASH,
    SEMICOLON,

    // Arithmetic Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    STAR_STAR,

    // Logical/Contextual
    AMPERSAND,
    LOGICAL_AND,
    LOGICAL_OR,

    // Relational
    GREATER_THAN,
    LESS_THAN,

    // Ternary
    QUESTION
};

class Token {
public:
    TokenType type;
    std::string value;

    // Helper for debugging
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
};

} // namespace CHTL

#endif // CHTL_TOKEN_H