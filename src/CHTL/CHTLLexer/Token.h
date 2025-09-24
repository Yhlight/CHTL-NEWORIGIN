#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Single-character tokens
    LEFT_BRACE, RIGHT_BRACE,
    COLON, SEMICOLON,
    COMMA,
    DOT,        // .
    HASH,       // #
    AMPERSAND,  // &

    // Literals
    IDENTIFIER,
    UNQUOTED_LITERAL,
    STRING,

    // Keywords
    KEYWORD_TEXT,
    KEYWORD_STYLE,

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
