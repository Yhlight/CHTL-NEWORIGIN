#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>

namespace CHTL {

enum class TokenType {
    // Define some basic token types
    UNKNOWN,
    END_OF_FILE,
    IDENTIFIER,
    STRING_LITERAL,
    COMMENT,
    TEXT_KEYWORD,
    L_BRACE,
    R_BRACE,
    STYLE_KEYWORD,
    SCRIPT_KEYWORD,
    COLON,
    EQUAL,
    SEMICOLON,
    NUMBER
};

class Token {
public:
    TokenType type;
    std::string value;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H