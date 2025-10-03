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

private:
    TokenType op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
};