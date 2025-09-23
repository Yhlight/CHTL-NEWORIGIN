#pragma once

#include "Token.h"
#include <string>
#include <vector>

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);
    Token getNextToken();
    Token peekToken();

    // Methods for raw string parsing
    const std::string& getSource() const;
    size_t getPosition() const;
    void setPosition(size_t pos);


private:
    Token generateNextToken();
    std::string source;
    size_t pos;
    size_t line;
    size_t column;

    // For peeking
    Token peekBuffer;
    bool isPeeked;

    // Helper methods
    char currentChar();
    char peekChar();
    void advance();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();

    Token makeToken(TokenType type, const std::string& value);
    Token identifier();
    Token number();
    Token stringLiteral(char quote);
    Token unquotedLiteral();
};
