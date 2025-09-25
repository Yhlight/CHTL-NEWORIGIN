#pragma once

#include "Token.h"
#include <string>

// The Lexer class is responsible for taking a string of CHTL source code
// and breaking it down into a sequence of Tokens.
class Lexer {
public:
    // Constructs a new Lexer with the given source code.
    Lexer(const std::string& source);

    // Returns the next token from the source code.
    Token getNextToken();

    // -- Raw string parsing helpers for the Parser --
    const std::string& getSource() const;
    size_t getPosition() const;
    void setPosition(size_t position);

private:
    std::string source;
    size_t position;
    int line;
    int column;

    // Helper methods for tokenization.
    char peek();
    char advance();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    Token number();
    Token identifier();
    Token stringLiteral(char quoteType);
};
