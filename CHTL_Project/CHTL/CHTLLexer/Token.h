#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    // Meta
    EndOfFile,
    Unknown,

    // Literals
    Identifier,      // e.g., div, myVar
    NumberLiteral,   // e.g., 100, 3.14
    UnquotedLiteral, // e.g., red, px
    StringLiteral,   // e.g., "hello world"

    // Symbols
    OpenBrace,       // {
    CloseBrace,      // }
    OpenParen,       // (
    CloseParen,      // )
    OpenBracket,     // [
    CloseBracket,    // ]
    Colon,           // :
    Semicolon,       // ;
    Equals,          // =
    Ampersand,       // &
    Comma,           // ,
    Dot,             // .
    Hash,            // #
    At,              // @

    // Operators for property arithmetic
    Plus,            // +
    Minus,           // -
    Asterisk,        // *
    Slash,           // /
    Percent,         // %
    DoubleAsterisk,  // **

    // Keywords (will be expanded)
    Text,
    Style,
    Script,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Use,
    Except,
    From,
    As,

    // Comments
    LineComment,
    BlockComment,
    GeneratorComment,
};

struct Token {
    TokenType type;
    std::string value;
    int line = 0;
    int column = 0;
    size_t char_index = 0; // The starting character index in the source string

    static std::string typeToString(TokenType type) {
        // Helper function for debugging
        const std::vector<std::string> typeStrings = {
            "EndOfFile", "Unknown", "Identifier", "NumberLiteral", "UnquotedLiteral", "StringLiteral",
            "OpenBrace", "CloseBrace", "OpenParen", "CloseParen", "OpenBracket", "CloseBracket",
            "Colon", "Semicolon", "Equals", "Ampersand", "Comma", "Dot", "Hash", "At",
            "Plus", "Minus", "Asterisk", "Slash", "Percent", "DoubleAsterisk",
            "Text", "Style", "Script", "Template", "Custom", "Origin", "Import",
            "Namespace", "Use", "Except", "From", "As",
            "LineComment", "BlockComment", "GeneratorComment"
        };
        return typeStrings[static_cast<int>(type)];
    }
};

} // namespace CHTL
