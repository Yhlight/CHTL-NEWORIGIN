#ifndef LEXER_H
#define LEXER_H

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
    Token readString(char delimiter);
    Token readComment();
    void skipSingleLineComment();
    void skipMultiLineComment();

    std::string input;
    size_t position;     // current position in input (points to current char)
    size_t readPosition; // current reading position in input (after current char)
    char ch;          // current char under examination
    int line;         // current line number
    int column;       // current column number for the start of the token
};

} // namespace CHTL

#endif // LEXER_H
