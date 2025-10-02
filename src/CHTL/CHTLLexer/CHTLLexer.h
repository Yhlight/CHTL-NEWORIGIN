#pragma once

#include "Token.h"
#include <string>
#include <vector>

class CHTLLexer {
public:
    CHTLLexer(const std::string& input);
    Token getNextToken();
    void setRawContentMode(bool enabled);

private:
    std::string input;
    bool rawContentMode;
    size_t position;
    int line;
    int column;

    char currentChar();
    void advance();
    void skipWhitespace();
    Token identifier();
    Token stringLiteral();
    Token unquotedLiteral();
    Token singleLineComment();
    Token multiLineComment();
    Token generatorComment();
};