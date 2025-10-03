#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"
#include <memory>

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(TokenType op, std::unique_ptr<ExpressionNode> left, std::unique_ptr<ExpressionNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}

    NodeType getType() const override { return NodeType::BinaryOp; }

    TokenType getOp() const { return op; }
    ExpressionNode* getLeft() const { return left.get(); }
    ExpressionNode* getRight() const { return right.get(); }

    std::unique_ptr<BaseNode> clone() const override {
        auto cloned_left = left ? static_cast<ExpressionNode*>(left->clone().release()) : nullptr;
        auto cloned_right = right ? static_cast<ExpressionNode*>(right->clone().release()) : nullptr;
        return std::make_unique<BinaryOpNode>(op, std::unique_ptr<ExpressionNode>(cloned_left), std::unique_ptr<ExpressionNode>(cloned_right));
    }

private:
    TokenType op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
};