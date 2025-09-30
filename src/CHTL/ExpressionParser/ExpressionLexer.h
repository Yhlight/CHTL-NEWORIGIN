#pragma once

#include "ExpressionToken.h"
#include <string>
#include <vector>

// The ExpressionLexer is responsible for breaking down a CHTL style
// expression string into a sequence of ExpressionTokens.
class ExpressionLexer {
public:
    explicit ExpressionLexer(const std::string& source);

    // Returns a vector of all tokens from the source string.
    std::vector<ExpressionToken> tokenize();

private:
    std::string source;
    size_t position;

    // Helper methods for tokenization.
    bool isAtEnd() const;
    ExpressionToken getNextToken();
    char peek() const;
    char advance();
    void skipWhitespace();
    ExpressionToken number();
    ExpressionToken identifier();
    ExpressionToken stringLiteral(char quoteType);
};