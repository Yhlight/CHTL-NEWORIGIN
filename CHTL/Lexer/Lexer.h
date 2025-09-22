#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();
    void setRawMode(bool is_raw);

private:
    bool raw_mode = false;
    std::string source;
    size_t position = 0;
    int line = 1;
    int column = 1;

    char currentChar() const;
    char peekChar() const;
    void advance();
    void skipWhitespace();
    Token makeIdentifier();
    Token makeString();
};

} // namespace CHTL
