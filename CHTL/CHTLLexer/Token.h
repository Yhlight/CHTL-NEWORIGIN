#pragma once

#include <string>

enum class TokenType {
    // ... to be filled in later
    EndOfFile,
    Unknown
};

class Token {
public:
    Token(TokenType type, std::string value) : type(type), value(std::move(value)) {}

    TokenType getType() const { return type; }
    const std::string& getValue() const { return value; }

private:
    TokenType type;
    std::string value;
};