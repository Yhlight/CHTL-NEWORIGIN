#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>

enum class TokenType {
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT,
    GENERATOR_COMMENT,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class CHTLLexer {
public:
    std::vector<Token> tokenize(const std::string& input);
};

#endif // CHTL_LEXER_H