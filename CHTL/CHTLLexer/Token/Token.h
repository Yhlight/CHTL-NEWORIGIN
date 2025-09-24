#pragma once
#include <string>
#include <string_view>

namespace chtl::lexer {

enum class TokenType {
    End,
    Identifier,
    StringLiteral,
    UnquotedLiteral,
    Number,
    LBrace,    // {
    RBrace,    // }
    LBracket,  // [
    RBracket,  // ]
    LParen,    // (
    RParen,    // )
    Comma,
    Semicolon,
    Colon,     // :
    Equal,     // = (CE 对等式)
    Hash,      // # (生成器注释前缀)
    At,        // @ (类型前缀)
    Slash,     // /
    Dot,       // . (路径对等式)
    Comment,   // // 或 /** */ 按原样收集

    // Block keywords []
    KwTemplate,
    KwCustom,
    KwOrigin,
    KwImport,
    KwConfiguration,
    KwNamespace,

    // Behaviors and common keys
    KwStyle,
    KwScript,
    KwText,
    KwUse,
    KwInherit,
    KwDelete,
    KwInsert,
    KwExcept,
    KwFrom,
    KwAs,
    KwHtml5,
};

struct Token {
    TokenType type { TokenType::End };
    std::string lexeme;
    std::size_t line { 0 };
    std::size_t column { 0 };
};

} // namespace chtl::lexer

