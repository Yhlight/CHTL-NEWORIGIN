#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& literal);
    bool isAtEnd();
    bool match(char expected);
    char peek();
    char peekNext();

    // Token scanning methods
    void scanString(char quote);
    void scanNumber();
    void scanIdentifier();
    void scanBlockComment();
    void scanLineComment();
    void scanGeneratorComment();
    void scanUnquotedLiteral(); // For things like `color: red;`

    const std::string& source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    int col_start = 1;

    static const std::map<std::string, TokenType> keywords;
};

} // namespace CHTL
