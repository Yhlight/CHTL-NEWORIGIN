#ifndef TOKEN_H
#define TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,

    // Identifiers
    IDENTIFIER,

    // Literals
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMBER,

    // Operators
    PLUS,         // +
    MINUS,        // -
    STAR,         // *
    SLASH,        // /
    PERCENT,      // %
    POWER,        // **

    // Symbols
    LEFT_BRACE,   // {
    RIGHT_BRACE,  // }
    COLON,        // :
    EQUALS,       // =
    SEMICOLON,    // ;
    DOT,          // .
    HASH,         // #
    AMPERSAND,    // &
    LEFT_BRACKET, // [
    RIGHT_BRACKET,// ]
    AT_SIGN,      // @

    // Comments
    COMMENT,

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

} // namespace CHTL

#endif // TOKEN_H
