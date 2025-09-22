#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& input);

    Token nextToken();

private:
    void readChar();
    char peekChar() const;
    void skipWhitespace();
    Token readIdentifier();
    Token readStringLiteral(char quote_type);
    Token readUnquotedLiteral();
    Token readLineComment();
    Token readBlockComment();
    bool isLetter(char ch) const;
    bool isDigit(char ch) const;
    bool isWhitespace(char ch) const;

    std::string input;
    size_t position;      // current position in input (points to current char)
    size_t readPosition;  // current reading position in input (after current char)
    char ch;              // current char under examination
    int line;
    int column;
};

} // namespace CHTL

#endif // CHTL_LEXER_H
