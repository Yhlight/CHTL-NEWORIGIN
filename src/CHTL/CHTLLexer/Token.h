#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    COLON, SEMICOLON,
    COMMA,

    // Literals
    IDENTIFIER,
    UNQUOTED_LITERAL,
    STRING,

    // Keywords (will be expanded later)
    KEYWORD_TEXT, // 'text'

    // Comments
    COMMENT,

    // End of File
    END_OF_FILE,

    // Unknown/Error
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

} // namespace CHTL
