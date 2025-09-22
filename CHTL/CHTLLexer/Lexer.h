#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string& source);
    Token getNextToken();
    std::vector<Token> getAllTokens();

private:
    std::string source;
    size_t position;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;

    char currentChar();
    void advance();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    Token handleGeneratorComment();
    Token handleStringLiteral();
    Token handleIdentifierOrKeyword();
    Token handleNumber();
    Token handleOperatorOrPunctuator();
    Token makeToken(TokenType type, const std::string& value);
    Token makeToken(TokenType type, char value);
};
