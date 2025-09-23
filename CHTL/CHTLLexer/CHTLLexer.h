#pragma once

#include "Token.h"
#include <string>
#include <vector>

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);
    Token getNextToken();
    Token peekToken();
    void markPosition();
    void rewindToMark();

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

    // For peeking and lookahead
    Token peekBuffer;
    bool isPeeked;
    size_t marked_pos;
    size_t marked_line;
    size_t marked_column;
    bool is_marked;


    // Helper methods
    char currentChar();
    char peekChar();
    void advance();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();

    Token makeToken(TokenType type, const std::string& value, size_t token_pos, size_t token_col);
    Token identifier(size_t start_pos, size_t start_col);
    Token number(size_t start_pos, size_t start_col);
    Token stringLiteral(char quote, size_t start_pos, size_t start_col);
};
