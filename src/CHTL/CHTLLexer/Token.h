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
    QuestionMark, // ?
    LogicalOr,    // ||
    LogicalAnd,   // &&
    EqualsEquals, // ==
    NotEquals,    // !=
    LessThan,     // <
    GreaterThan,  // >
    LessThanEquals, // <=
    GreaterThanEquals, // >=

    // Parentheses
    OpenParen,   // (
    CloseParen,  // )

    // CSS Selectors
    Dot,         // .
    Hash,        // # (for selectors)
    Ampersand,   // &

    // Template Syntax
    OpenBracket,  // [
    CloseBracket, // ]
    At,           // @
    Inherit,      // inherit

    // Punctuation
    Comma,        // ,

    // Keywords
    Import,       // Import
    Origin,       // Origin
    Use,          // use
    Namespace,    // Namespace
    From,         // from
    Custom,       // Custom
    Delete,       // delete
    Insert,       // insert
    After,        // after
    Before,       // before
    Replace       // replace
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
};
