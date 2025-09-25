#pragma once

#include <string>

namespace CHTLJS {

enum class TokenType {
    // Special Tokens
    EndOfFile,
    Unexpected,

    // Literals
    Identifier, // e.g., myFunc, my_var
    String,     // "a string"
    Number,     // 123, 45.6

    // Keywords & Operators
    Arrow,      // ->
    Listen,     // Listen
    Delegate,   // Delegate
    Animate,    // Animate
    Vir,        // Vir
    Router,     // Router

    // Punctuation
    OpenBrace,  // {
    CloseBrace, // }
    OpenParen,  // (
    CloseParen, // )
    Colon,      // :
    Comma,      // ,
    Semicolon,  // ;

    // Enhanced Selector
    OpenDoubleBrace, // {{
    CloseDoubleBrace, // }}
};

struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
};

} // namespace CHTLJS