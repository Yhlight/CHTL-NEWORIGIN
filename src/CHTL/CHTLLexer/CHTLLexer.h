#pragma once

#include "Token.h"
#include <string>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& source);

    Token getNextToken();

private:
    char peek() const;
    char advance();
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token makeToken(TokenType type, const std::string& lexeme) const;
    Token makeIdentifier();
    Token makeString(char quoteType);

    std::string _source;
    size_t _start = 0;
    size_t _current = 0;
    int _line = 1;
    int _column = 1;
};

} // namespace CHTL
