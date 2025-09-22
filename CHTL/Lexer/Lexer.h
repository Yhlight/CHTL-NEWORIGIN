#pragma once

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> scan_tokens();

private:
    Token scan_token();
    char advance();
    bool is_at_end() const;
    char peek() const;
    char peek_next() const;
    bool match(char expected);
    void skip_whitespace_and_comments();

    Token make_token(TokenType type) const;
    Token error_token(const std::string& message) const;

    Token string_literal(char quote_type);
    Token identifier();
    Token number();

    const std::string source;
    size_t start = 0;
    size_t current = 0;
    int line = 1;
    int column = 1;
};
