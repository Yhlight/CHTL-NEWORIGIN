#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    const std::string& source;
    int current = 0;
    int line = 1;
    int column = 1;

    char advance();
    char peek();
    char peek_next();
    bool is_at_end();

    Token make_token(TokenType type, const std::string& value);
    Token make_token(TokenType type);

    Token scan_token();
    Token identifier();
    Token string_literal();
    Token number();

    void skip_whitespace_and_comments();
};
