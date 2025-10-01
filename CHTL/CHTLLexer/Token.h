#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <cstddef>

namespace CHTL {

enum class TokenType {
    // Basic
    UNKNOWN,
    EndOfFile,
    IDENTIFIER,
    STRING_LITERAL,
    HEX_LITERAL,
    NUMBER,
    COMMENT,

    // Keywords
    TEXT_KEYWORD,
    STYLE_KEYWORD,
    SCRIPT_KEYWORD,
    USE_KEYWORD,
    HTML5_KEYWORD,
    INHERIT_KEYWORD,
    CUSTOM_KEYWORD,
    ORIGIN_KEYWORD,
    DELETE_KEYWORD,
    INSERT_KEYWORD,
    AFTER_KEYWORD,
    BEFORE_KEYWORD,
    REPLACE_KEYWORD,
    AT_KEYWORD,
    TOP_KEYWORD,
    BOTTOM_KEYWORD,
    IF_KEYWORD,
    ELSE_KEYWORD,
    CONDITION_KEYWORD,

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
    L_PAREN,
    R_PAREN,
    COMMA,

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
    size_t pos;

    // Helper for debugging
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
};

} // namespace CHTL

#endif // CHTL_TOKEN_H