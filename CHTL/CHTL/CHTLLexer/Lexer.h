#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
    void lineComment();
    void blockComment();
    Token makeToken(TokenType type);
    Token makeToken(TokenType type, const std::string& lexeme);
    Token errorToken(const std::string& message);

    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;

    static const std::unordered_map<std::string, TokenType> keywords;
};

} // namespace CHTL