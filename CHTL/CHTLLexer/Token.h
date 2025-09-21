#pragma once

#include <string>

namespace CHTL {

// Enum class for strongly-typed token identifiers
enum class TokenType {
    // Single-character tokens
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    COLON,           // :
    SEMICOLON,       // ;
    EQUAL,           // =

    // Literals
    IDENTIFIER,      // e.g., div, id, my-class, or an unquoted literal like 'red'
    STRING,          // "content" or 'content'. The lexeme will contain the content without quotes.

    // Comments
    COMMENT,         // For //, /* */, and # comments. The lexeme will contain the full comment line/block (e.g., "// text" or "# text").

    // Special Tokens
    TOKEN_EOF,       // Represents the end of the source file.
    TOKEN_ERROR,     // Represents a lexing error. The lexeme will contain the error message.
};

// Helper function to convert TokenType to a string, useful for debugging.
inline const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE:   return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:  return "RIGHT_BRACE";
        case TokenType::COLON:        return "COLON";
        case TokenType::SEMICOLON:    return "SEMICOLON";
        case TokenType::EQUAL:        return "EQUAL";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::STRING:       return "STRING";
        case TokenType::COMMENT:      return "COMMENT";
        case TokenType::TOKEN_EOF:    return "EOF";
        case TokenType::TOKEN_ERROR:  return "ERROR";
        default:                      return "UNKNOWN";
    }
}

// Represents a single token produced by the Lexer.
struct Token {
    TokenType type;
    std::string lexeme; // The actual substring from the source code that this token represents.
    int line;           // The line number where the token starts.
    int column;         // The column number where the token starts.
};

} // namespace CHTL
