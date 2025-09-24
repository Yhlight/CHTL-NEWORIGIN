#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include "Token.h"

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    void scanToken();
    char advance();
    bool isAtEnd();
    char peek();
    char peekNext();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);

    void processString();
    void processIdentifier();

    const std::string& source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
};

#endif // CHTL_LEXER_H
