#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <map>

namespace CHTL {

class Lexer {
public:
    // Constructor: Initializes the lexer with the source code to be tokenized.
    Lexer(std::string input);

    // Returns the next token from the input string.
    Token nextToken();

private:
    // Helper methods for processing the input string.
    void readChar();         // Reads the next character and advances the position.
    char peekChar();         // Looks at the next character without consuming it.
    void skipWhitespace();   // Skips over whitespace characters.

    // Helper methods for reading specific types of tokens.
    Token readIdentifier();
    Token readStringLiteral();
    Token readComment();
    Token readUnquotedLiteral();

    std::string m_input;      // The input source code.
    size_t m_position = 0;    // Current position in input (points to current char).
    size_t m_readPosition = 0;// Current reading position (points to the next char).
    char m_ch = 0;            // The character currently being examined.
    int m_line = 1;           // Current line number for error reporting.
};

} // namespace CHTL

#endif // CHTL_LEXER_H
