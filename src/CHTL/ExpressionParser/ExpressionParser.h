#pragma once

#include "ExpressionToken.h"
#include "../ExpressionNode/ExpressionBaseNode.h"
#include <vector>
#include <memory>

// The ExpressionParser takes a sequence of ExpressionTokens and constructs
// an Abstract Syntax Tree (AST) representing the expression.
class ExpressionParser {
public:
    explicit ExpressionParser(const std::vector<ExpressionToken>& tokens);

    // The main entry point for parsing.
    std::unique_ptr<ExpressionBaseNode> parse();

private:
    std::vector<ExpressionToken> tokens;
    size_t position = 0;

    // Helper methods for Pratt parsing (or similar precedence-climbing methods).
    std::unique_ptr<ExpressionBaseNode> parseExpression(int precedence = 0);
    std::unique_ptr<ExpressionBaseNode> parsePrefix();
    std::unique_ptr<ExpressionBaseNode> parseInfix(std::unique_ptr<ExpressionBaseNode> left, const ExpressionToken& token);

    const ExpressionToken& peek() const;
    const ExpressionToken& advance();
    bool isAtEnd() const;
    int getPrecedence(const ExpressionToken& token) const;
};