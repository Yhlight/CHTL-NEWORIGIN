#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t current = 0;
    size_t line = 1;
    size_t column = 1;

    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);
    void skipWhitespace();

    Token makeToken(TokenType type);
    Token makeToken(TokenType type, const std::string& lexeme);
    Token errorToken(const std::string& message);

    Token scanToken();
    Token scanIdentifier();
    Token scanStringLiteral(char quoteType);
    Token scanNumberLiteral();
    Token scanComment();
    TokenType identifierType(const std::string& identifier);
};

} // namespace CHTL
