#pragma once

#include <memory>
#include "../CHTLLexer/Token.h"

// Forward declarations to avoid circular dependencies
class CHTLParser;
class ExpressionNode;

class ExpressionParser {
public:
    ExpressionParser(CHTLParser& parser);
    std::unique_ptr<ExpressionNode> parse();

private:
    std::unique_ptr<ExpressionNode> parseExpression(int precedence);
    std::unique_ptr<ExpressionNode> parsePrefix();
    std::unique_ptr<ExpressionNode> parseInfix(std::unique_ptr<ExpressionNode> left);
    int getPrecedence();

    CHTLParser& parser;
};