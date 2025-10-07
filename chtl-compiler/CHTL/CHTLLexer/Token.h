#pragma once

#include <string>
#include <vector>

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unknown,

    // Literals
    Identifier,
    StringLiteral,
    UnquotedLiteral,

    // Punctuation
    LeftBrace,  // {
    RightBrace, // }
    Semicolon,  // ;
    Colon,      // :
    Equal,      // =

    // Comments
    LineComment,      // //
    BlockComment,     // /* ... */
    GeneratorComment, // #
};

struct Token {
    TokenType type;
    std::string value;
    // Basic position info for now
    int line;
    int column;
};