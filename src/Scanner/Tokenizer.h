#pragma once

#include <string>
#include <vector>

enum class TokenType {
    UNKNOWN,
    WHITESPACE,
    COMMENT,
    STRING_LITERAL,
    NUMBER_LITERAL,  // Added for numbers
    CHTL_KEYWORD,    // Animate, Listen, Router, etc.
    CHTL_SELECTOR,   // {{...}}
    CHTL_OPERATOR,   // ->
    VIR_KEYWORD,
    IDENTIFIER,      // e.g., mode, url, page, or any JS variable
    SYMBOL,          // {, }, :, ,, ;, =, etc.
    JS_BLOCK         // A token type used by the compiler for its output
};

struct Token {
    TokenType type;
    std::string text;
};

class Tokenizer {
public:
    std::vector<Token> tokenize(const std::string& source);
};