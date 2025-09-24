#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);

    // Tokenizes the entire source code and returns a vector of tokens.
    std::vector<Token> tokenizeAll();

private:
    const std::string& source;
    std::vector<Token> tokens;

    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;

    // Keyword map for easy lookup
    static const std::unordered_map<std::string, TokenType> keywords;

    // Main scanning loop
    void scanToken();

    // Character helpers
    bool isAtEnd();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

    // Token type helpers
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);

    // Specific token scanners
    void stringLiteral();
    void numberLiteral();
    void identifier();
    void lineComment();
    void blockComment();
    void genComment();
    void bracketedKeyword();

    // Utility
    void skipWhitespace();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
};

} // namespace CHTL
