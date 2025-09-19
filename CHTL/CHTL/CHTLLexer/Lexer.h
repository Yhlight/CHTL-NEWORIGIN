#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include "Token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    Token nextToken();

private:
    void skipWhitespace();
    Token readIdentifier();
    Token readString();
    Token readNumber();
    char peek();
    char peekNext();
    char advance();
    bool isAtEnd();
    bool isAlpha(char c);
    bool isDigit(char c);

    std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

#endif // CHTL_LEXER_H
