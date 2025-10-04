#include "ConditionalNode.h"

ConditionalNode::ConditionalNode(std::unique_ptr<ExpressionNode> condition,
                                 std::unique_ptr<ExpressionNode> trueExpression,
                                 std::unique_ptr<ExpressionNode> falseExpression)
    : condition(std::move(condition)),
      trueExpression(std::move(trueExpression)),
      falseExpression(std::move(falseExpression)) {}

NodeType ConditionalNode::getType() const {
    return NodeType::Conditional;
}

ExpressionNode* ConditionalNode::getCondition() const {
    return condition.get();
}

ExpressionNode* ConditionalNode::getTrueExpression() const {
    return trueExpression.get();
}

ExpressionNode* ConditionalNode::getFalseExpression() const {
    return falseExpression.get();
}

std::unique_ptr<BaseNode> ConditionalNode::clone() const {
    return std::make_unique<ConditionalNode>(
        std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(condition->clone().release())),
        std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(trueExpression->clone().release())),
        std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(falseExpression->clone().release()))
    );
}