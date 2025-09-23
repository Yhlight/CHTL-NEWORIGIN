#pragma once

#include <string>

// Defines the different types of tokens that the Lexer can produce.
enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier, // e.g., div, my_element
    String,     // e.g., "hello world"

    // Punctuation
    OpenBrace,  // {
    CloseBrace, // }
    Colon,      // :
    Equals,     // =
    Semicolon,  // ;

    // Comments
    HashComment // # a special comment to be processed
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
};
