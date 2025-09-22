#pragma once

#include "Token.h"
#include <string>

class Lexer {
public:
    Lexer(const std::string& input);

    Token NextToken();

public:
    const std::string& getInput() const { return input; }
    size_t getReadPosition() const { return readPosition; }
    size_t getPosition() const { return position; }
    void setSkipWhitespace(bool skip) { _skipWhitespace = skip; }

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    Token readIdentifier();
    Token readStringLiteral();
    Token readUnquotedLiteral();
    Token readNumberLiteral();
    TokenType lookupIdent(const std::string& ident);

    std::string input;
    bool _skipWhitespace;
    size_t position;      // current position in input (points to current char)
    size_t readPosition;  // current reading position in input (after current char)
    char ch;              // current char under examination
    int line;             // current line number
    int column;           // current column number
};
