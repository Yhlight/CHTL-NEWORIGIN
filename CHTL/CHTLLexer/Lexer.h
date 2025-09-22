#pragma once

#include "Token.h"
#include <string>
#include <vector>

class CHTLLexer {
public:
    explicit CHTLLexer(std::string source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    bool isAtEnd();
    char peek();
    char peekNext();
    bool match(char expected);

    void stringLiteral(char quote_type);
    void identifier();
    void singleLineComment();
    void multiLineComment();
    void generatorComment();
    void unquotedLiteral();


    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;
};
