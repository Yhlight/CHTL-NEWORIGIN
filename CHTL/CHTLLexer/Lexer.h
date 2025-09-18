#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    const std::string& source;
    size_t current = 0;
    size_t start = 0;
    int line = 1;
    int column = 1;

    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);
    void skipWhitespaceAndNewlines();

    Token makeToken(TokenType type);
    Token makeToken(TokenType type, const std::string& value);
    Token stringLiteral();
    Token identifier();
    Token processSymbol();
    Token processSlash(); // For comments or division
};

} // namespace CHTL
