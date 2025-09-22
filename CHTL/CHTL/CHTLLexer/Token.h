#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

// Defines the type of a token.
enum class TokenType {
    // Special Tokens
    ILLEGAL,      // An unknown or invalid token
    END_OF_FILE,  // EOF

    // Identifiers & Literals
    IDENTIFIER,       // e.g., div, color, my_template
    STRING_LITERAL,   // "a string" or 'a string'
    UNQUOTED_LITERAL, // red, 100px, solid

    // Operators and Delimiters
    LEFT_BRACE,       // {
    RIGHT_BRACE,      // }
    COLON,            // :
    EQUALS,           // =
    SEMICOLON,        // ;
    DOT,              // .
    HASH,             // #
    AT_SIGN,          // @
    LEFT_BRACKET,     // [
    RIGHT_BRACKET,    // ]
    LEFT_PAREN,       // (
    RIGHT_PAREN,      // )

    // Keywords
    DELETE,           // delete
    FROM,             // from

    // Comments - The lexer will identify them, parser might ignore some.
    COMMENT,          // // a comment, /* a comment */, # a comment
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string literal;
    int line = 0; // The line number where the token appears
};

// Helper function for debugging to get a string representation of a token type.
inline std::string TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::COLON: return ":";
        case TokenType::EQUALS: return "=";
        case TokenType::SEMICOLON: return ";";
        case TokenType::DOT: return ".";
        case TokenType::HASH: return "#";
        case TokenType::AT_SIGN: return "@";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::DELETE: return "DELETE";
        case TokenType::FROM: return "FROM";
        case TokenType::COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

} // namespace CHTL

#endif // CHTL_TOKEN_H
