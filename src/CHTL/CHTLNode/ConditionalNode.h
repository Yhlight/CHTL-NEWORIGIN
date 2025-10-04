#pragma once

#include "ExpressionNode.h"
#include <memory>

class ConditionalNode : public ExpressionNode {
public:
    ConditionalNode(std::unique_ptr<ExpressionNode> condition,
                    std::unique_ptr<ExpressionNode> trueExpression,
                    std::unique_ptr<ExpressionNode> falseExpression);

    virtual NodeType getType() const override;

    ExpressionNode* getCondition() const;
    ExpressionNode* getTrueExpression() const;
    ExpressionNode* getFalseExpression() const;

    virtual std::unique_ptr<BaseNode> clone() const override;

private:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> trueExpression;
    std::unique_ptr<ExpressionNode> falseExpression;
};