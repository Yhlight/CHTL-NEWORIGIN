#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <variant>
#include <unordered_map>
#include <memory>

namespace chtl {

struct SourceLocation {
    size_t line {1};
    size_t column {1};
};

struct SourceRange {
    SourceLocation begin{};
    SourceLocation end{};
};

struct Error {
    std::string message;
    SourceRange range{};
};

enum class TokenKind {
    EndOfFile,
    Identifier,
    String,
    Number,
    LBrace,     // {
    RBrace,     // }
    LParen,     // (
    RParen,     // )
    LBracket,   // [
    RBracket,   // ]
    Less,       // <
    Greater,    // >
    Slash,      // /
    Colon,      // :
    Equal,      // =
    Semicolon,  // ;
    Comma,      // ,
    Dot,        // .
    Hash,       // #
    At,         // @
    Text,
    Comment,
};

struct Token {
    TokenKind kind{TokenKind::EndOfFile};
    std::string lexeme{};
    SourceRange range{};
};

} // namespace chtl
