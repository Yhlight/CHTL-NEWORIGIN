#pragma once

#include <string>

// Defines the different types of tokens that the Lexer can produce.
enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Number,     // e.g., 100, 3.14
    Identifier, // e.g., div, my_element
    String,     // e.g., "hello world"

    // Punctuation
    OpenBrace,  // {
    CloseBrace, // }
    Colon,      // :
    Equals,     // =
    Semicolon,  // ;

    // Comments
    HashComment, // # a special comment to be processed

    // Operators
    Plus,        // +
    Minus,       // -
    Asterisk,    // *
    Slash,       // /
    Percent,     // %

    // Parentheses
    OpenParen,   // (
    CloseParen   // )
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
};
