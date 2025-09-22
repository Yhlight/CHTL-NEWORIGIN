#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

class Lexer {
public:
    Lexer(const std::string& input);

    std::vector<Token> tokenize();

private:
    std::string source;
    size_t position = 0;
    int line = 1;
    int column = 1;

    char peek(size_t offset = 0);
    char advance();
    bool isAtEnd();
    void skipWhitespace();

    Token scanToken();
    Token makeIdentifierOrKeyword();
    Token makeStringLiteral(char quote_type);
    Token makeNumberLiteral();
    Token makeUnquotedLiteral();
    Token handleComment();
    Token handleSlash();
    Token handleAsterisk();
};

} // namespace CHTL
