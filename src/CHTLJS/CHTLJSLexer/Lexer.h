#pragma once

#include "Token.h"
#include <string>

namespace CHTLJS {

class Lexer {
public:
    explicit Lexer(const std::string& source);

    Token getNextToken();

private:
    std::string source;
    size_t position;
    int line;
    int column;

    char peek();
    char advance();
    void skipWhitespace();
    Token identifier();
    Token stringLiteral();
    Token number();
};

} // namespace CHTLJS