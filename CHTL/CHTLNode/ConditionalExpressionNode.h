#ifndef CHTL_CONDITIONAL_EXPRESSION_NODE_H
#define CHTL_CONDITIONAL_EXPRESSION_NODE_H

#include "ExpressionNode.h"
#include <memory>

namespace CHTL {

class ConditionalExpressionNode : public ExpressionNode {
public:
    ConditionalExpressionNode(
        std::unique_ptr<ExpressionNode> condition,
        std::unique_ptr<ExpressionNode> true_expression,
        std::unique_ptr<ExpressionNode> false_expression);

    ExpressionType getType() const override;
    void print(int indent = 0) const override;

    const ExpressionNode* getCondition() const;
    const ExpressionNode* getTrueExpression() const;
    const ExpressionNode* getFalseExpression() const;

private:
    std::unique_ptr<ExpressionNode> m_condition;
    std::unique_ptr<ExpressionNode> m_true_expression;
    std::unique_ptr<ExpressionNode> m_false_expression;
};

} // namespace CHTL

#endif // CHTL_CONDITIONAL_EXPRESSION_NODE_H