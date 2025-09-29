#pragma once

#include "ExpressionBaseNode.h"
#include <memory>

// Represents a conditional (ternary) expression, e.g., "condition ? true_expr : false_expr".
class ConditionalNode : public ExpressionBaseNode {
public:
    std::unique_ptr<ExpressionBaseNode> condition;
    std::unique_ptr<ExpressionBaseNode> trueExpression;
    std::unique_ptr<ExpressionBaseNode> falseExpression;

    ConditionalNode(
        std::unique_ptr<ExpressionBaseNode> cond,
        std::unique_ptr<ExpressionBaseNode> trueExpr,
        std::unique_ptr<ExpressionBaseNode> falseExpr)
        : condition(std::move(cond)),
          trueExpression(std::move(trueExpr)),
          falseExpression(std::move(falseExpr)) {}

    ExpressionNodeType getType() const override {
        return ExpressionNodeType::Conditional;
    }
};