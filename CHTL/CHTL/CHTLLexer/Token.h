#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Special Tokens
    ILLEGAL, // Represents an illegal/unknown token
    END_OF_FILE,

    // Identifiers & Literals
    IDENTIFIER,     // e.g., div, my_variable
    STRING_LITERAL, // "hello world" or 'hello world'
    UNQUOTED_LITERAL, // hello (as a value)

    // Delimiters
    LBRACE,     // {
    RBRACE,     // }
    LPAREN,     // (
    RPAREN,     // )
    LBRACKET,   // [
    RBRACKET,   // ]

    // Operators
    COLON,      // :
    SEMICOLON,  // ;
    EQUALS,     // =
    COMMA,      // ,

    // Comments
    LINE_COMMENT, // // a comment
    BLOCK_COMMENT, // /* a comment */

    // Keywords will be added later
};

struct Token {
    TokenType type;
    std::string literal;
    int line;
    int column;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H
