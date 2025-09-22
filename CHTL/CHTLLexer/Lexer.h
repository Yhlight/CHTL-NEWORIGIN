#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

class Lexer {
public:
    explicit Lexer(std::string input);
    Token NextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    Token readIdentifier();
    Token readString(char quote);
    Token readUnquotedString();

    std::string m_input;
    size_t m_position;      // current position in input (points to current char)
    size_t m_readPosition;  // current reading position in input (after current char)
    char m_char;            // current char under examination
};

} // CHTL
