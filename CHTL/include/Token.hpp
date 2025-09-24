#pragma once

#include <string>

namespace chtl::core {

enum class TokenType {
    Identifier,
    String,
    Number,
    LBrace,
    RBrace,
    Colon,
    Equal,
    Semicolon,
    Comment,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

}
