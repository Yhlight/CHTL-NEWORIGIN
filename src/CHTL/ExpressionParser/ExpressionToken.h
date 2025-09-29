#pragma once

#include <string>

// Defines the token types for the CHTL expression language.
enum class ExpressionTokenType {
    // Literals
    Number,      // e.g., 100, 3.14
    Identifier,  // e.g., "px", "em", property names
    String,      // e.g., "'red'", "\"blue\""

    // Operators
    Plus,        // +
    Minus,       // -
    Asterisk,    // *
    Slash,       // /
    Percent,     // %
    DoubleAsterisk, // **

    // Logical Operators
    LogicalAnd,        // &&
    LogicalOr,         // ||
    EqualsEquals,      // ==
    NotEquals,         // !=
    GreaterThan,       // >
    LessThan,          // <
    GreaterThanEquals, // >=
    LessThanEquals,    // <=

    // Punctuation
    Dot,          // .
    OpenParen,    // (
    CloseParen,   // )
    QuestionMark, // ?
    Colon,        // :

    // Special
    EndOfFile,
    Unexpected
};

// Represents a single token extracted from an expression string.
struct ExpressionToken {
    ExpressionTokenType type;
    std::string value;
};