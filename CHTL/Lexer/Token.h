#pragma once

#include <string>
#include <variant>

namespace CHTL {

enum class TokenType {
    Identifier,
    LeftBrace,
    RightBrace,
    TextKeyword,
    StringLiteral,
    EndOfFile,
    Semicolon,
    Colon,
    Equals,
    Dot,
    Hash,
    Ampersand,
    LeftBracket,
    RightBracket,
    At,
    InheritKeyword,
    Unknown,
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;

    static std::string typeToString(TokenType type) {
        switch (type) {
            case TokenType::Identifier:    return "Identifier";
            case TokenType::LeftBrace:     return "LeftBrace";
            case TokenType::RightBrace:    return "RightBrace";
            case TokenType::TextKeyword:   return "TextKeyword";
            case TokenType::StringLiteral: return "StringLiteral";
            case TokenType::EndOfFile:     return "EndOfFile";
            case TokenType::Semicolon:     return "Semicolon";
            case TokenType::Colon:         return "Colon";
            case TokenType::Equals:        return "Equals";
            case TokenType::Dot:           return "Dot";
            case TokenType::Hash:          return "Hash";
            case TokenType::Ampersand:     return "Ampersand";
            case TokenType::LeftBracket:   return "LeftBracket";
            case TokenType::RightBracket:  return "RightBracket";
            case TokenType::At:            return "At";
            case TokenType::InheritKeyword:return "InheritKeyword";
            case TokenType::Unknown:       return "Unknown";
        }
        return "Unknown";
    }
};

} // namespace CHTL
