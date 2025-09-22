#ifndef CHTL_VALUE_LEXER_H
#define CHTL_VALUE_LEXER_H

#include <string>
#include <vector>

enum class ValueTokenType {
    Number,
    Unit,
    Operator,
    EndOfValue,
    Unknown
};

struct ValueToken {
    ValueTokenType type;
    std::string value;
};

class ValueLexer {
public:
    explicit ValueLexer(const std::string& source);
    std::vector<ValueToken> tokenize();

private:
    std::string source;
    size_t current = 0;

    ValueToken nextToken();
    void skipWhitespace();
    char peek();
    char advance();
    bool isAtEnd();
};

#endif //CHTL_VALUE_LEXER_H
