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
    Token readString(char quote);
    void skipSingleLineComment();
    void skipMultiLineComment();

    std::string m_input;
    size_t m_position;
    size_t m_readPosition;
    char m_char;
};

} // CHTL
