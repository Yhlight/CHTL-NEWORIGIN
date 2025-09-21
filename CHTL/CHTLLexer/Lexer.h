#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    // Constructor that takes the source code to be tokenized.
    explicit Lexer(const std::string& source);

    // The main function that scans the entire source code and returns a vector of tokens.
    std::vector<Token> scanTokens();

private:
    // Scans and returns the next single token from the source.
    Token scanToken();

    // Helper methods for processing different types of tokens.
    Token makeString(char quoteType);
    Token makeIdentifier();
    Token makeComment();

    // Utility methods for consuming and inspecting characters.
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    void skipWhitespace();

    // Helper methods for creating tokens.
    Token makeToken(TokenType type) const;
    Token errorToken(const std::string& message) const;

    // Checks if the lexer has reached the end of the source code.
    bool isAtEnd() const;

    // Checks if a character can be part of an identifier.
    static bool isIdentifierChar(char c);

    const std::string& source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;
};

} // namespace CHTL
