#include "ConditionalNode.h"
#include <iostream>

namespace CHTL {

ConditionalNode::ConditionalNode(std::unique_ptr<ExpressionNode> condition,
                                 std::unique_ptr<ExpressionNode> trueExpression,
                                 std::unique_ptr<ExpressionNode> falseExpression)
    : condition(std::move(condition)),
      trueExpression(std::move(trueExpression)),
      falseExpression(std::move(falseExpression)) {}

ExpressionType ConditionalNode::getType() const {
    return ExpressionType::CONDITIONAL;
}

void ConditionalNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "Conditional {\n";

    for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
    std::cout << "Condition:\n";
    condition->print(indent + 2);

    for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
    std::cout << "True Branch:\n";
    trueExpression->print(indent + 2);

    for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
    std::cout << "False Branch:\n";
    falseExpression->print(indent + 2);

    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "}\n";
}

const ExpressionNode* ConditionalNode::getCondition() const {
    return condition.get();
}

const ExpressionNode* ConditionalNode::getTrueExpression() const {
    return trueExpression.get();
}

const ExpressionNode* ConditionalNode::getFalseExpression() const {
    return falseExpression.get();
}

} // namespace CHTL