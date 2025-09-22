#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();

private:
    std::string source;
    size_t position = 0;
    int line = 1;
    int column = 1;

    char currentChar() const;
    void advance();
    void skipWhitespaceAndComments();
    Token makeIdentifier();
    Token makeString();
};

} // namespace CHTL
