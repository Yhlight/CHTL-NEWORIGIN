#pragma once

#include "Token.h"
#include "Context.h" // Changed from CHTLContext/Context.h
#include <string>
#include <vector>
#include <map>

class Lexer {
public:
    Lexer(const std::string& source, const CHTLContext& context);
    std::vector<Token> scan_tokens();

private:
    const std::string& source;
    const CHTLContext& context;
    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;
    int col = 1;

    bool is_at_end();
    char advance();
    char peek();
    char peek_next();
    bool match(char expected);

    void scan_token();
    void add_token(TokenType type);
    void add_token(TokenType type, const std::string& literal);

    void skip_whitespace_and_comments();
    void handle_string(char quote_type);
    void handle_number();
    void handle_identifier();
    void handle_hash_comment();
};
