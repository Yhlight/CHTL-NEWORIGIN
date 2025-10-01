#include "ConditionalExpressionNode.h"
#include <iostream>

namespace CHTL {

ConditionalExpressionNode::ConditionalExpressionNode(
    std::unique_ptr<ExpressionNode> condition,
    std::unique_ptr<ExpressionNode> true_expression,
    std::unique_ptr<ExpressionNode> false_expression)
    : m_condition(std::move(condition)),
      m_true_expression(std::move(true_expression)),
      m_false_expression(std::move(false_expression)) {}

ExpressionType ConditionalExpressionNode::getType() const {
    return ExpressionType::CONDITIONAL;
}

void ConditionalExpressionNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Condition:" << std::endl;
    m_condition->print(indent + 1);

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "True Branch:" << std::endl;
    m_true_expression->print(indent + 1);

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "False Branch:" << std::endl;
    m_false_expression->print(indent + 1);
}

const ExpressionNode* ConditionalExpressionNode::getCondition() const {
    return m_condition.get();
}

const ExpressionNode* ConditionalExpressionNode::getTrueExpression() const {
    return m_true_expression.get();
}

const ExpressionNode* ConditionalExpressionNode::getFalseExpression() const {
    return m_false_expression.get();
}

} // namespace CHTL