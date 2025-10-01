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
    COMMENT,

    // Keywords
    KEYWORD_TEMPLATE,
    KEYWORD_STYLE,
    KEYWORD_ELEMENT,
    KEYWORD_VAR,
    KEYWORD_TEXT,
    KEYWORD_SCRIPT,
    KEYWORD_CUSTOM,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_USE,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_INHERIT,


    // Punctuation
    L_BRACE,    // {
    R_BRACE,    // }
    L_BRACKET,  // [
    R_BRACKET,  // ]
    COLON,      // :
    EQUAL,      // =
    SEMICOLON,  // ;
    AT_SIGN,    // @

    // Arithmetic Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    STAR_STAR,
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