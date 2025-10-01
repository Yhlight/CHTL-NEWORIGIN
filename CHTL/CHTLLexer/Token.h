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
    RAW_SCRIPT,
    COMMENT,

    // Keywords
    TEXT_KEYWORD,
    STYLE_KEYWORD,
    SCRIPT_KEYWORD,

    // Punctuation
    L_BRACE,
    R_BRACE,
    COLON,
    EQUAL,
    SEMICOLON,
    DOT,
    HASH,
    AMPERSAND,
    L_BRACKET,
    R_BRACKET,
    AT_SIGN,
    L_DOUBLE_BRACE,
    R_DOUBLE_BRACE,

    // Arithmetic Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    STAR_STAR,

    // Conditional and Logical Operators
    QUESTION_MARK,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,
    EQUAL_EQUAL,
    BANG_EQUAL,
    AMPERSAND_AMPERSAND,
    PIPE_PIPE
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