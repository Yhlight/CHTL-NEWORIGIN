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
    OpenDoubleBrace, // {{
    CloseDoubleBrace, // }}
    Dollar,     // $
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

    // Conditional and Logical Operators
    QuestionMark,      // ?
    LogicalAnd,        // &&
    LogicalOr,         // ||
    EqualsEquals,      // ==
    NotEquals,         // !=
    GreaterThan,       // >
    LessThan,          // <
    GreaterThanEquals, // >=
    LessThanEquals,    // <=

    // Keywords
    Origin,       // Origin
    Use,          // use
    Namespace,    // Namespace
    From,         // from
    Custom,       // Custom
    Delete,       // delete
    Insert,       // insert
    After,        // after
    Before,       // before
    Replace,      // replace
    AtTop,        // at top
    AtBottom,     // at bottom
    Import,       // [Import]
    Configuration, // [Configuration]
    Except        // except
};

// Represents a single token extracted from the source code.
struct Token {
    TokenType type;
    std::string value;
    int line = 1;
    int column = 1;
    size_t start_pos = 0; // The starting position of the token in the source string
};
