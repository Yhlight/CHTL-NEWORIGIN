#pragma once

#include "ExpressionNode.h"
#include <memory>

class ConditionalNode : public ExpressionNode {
public:
    ConditionalNode(std::unique_ptr<ExpressionNode> condition,
                    std::unique_ptr<ExpressionNode> true_expr,
                    std::unique_ptr<ExpressionNode> false_expr)
        : condition(std::move(condition)),
          true_expr(std::move(true_expr)),
          false_expr(std::move(false_expr)) {}

    NodeType getType() const override { return NodeType::Conditional; }

    const ExpressionNode* getCondition() const { return condition.get(); }
    const ExpressionNode* getTrueExpression() const { return true_expr.get(); }
    const ExpressionNode* getFalseExpression() const { return false_expr.get(); }

private:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> true_expr;
    std::unique_ptr<ExpressionNode> false_expr;
};