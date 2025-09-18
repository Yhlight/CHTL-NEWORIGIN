#pragma once

#include "ExpressionNode.h"
#include "../../CHTLLexer/Token.h"

namespace CHTL {

// Represents a binary operation in an expression tree, e.g., left + right.
class BinaryOpExpression : public ExpressionNode {
public:
    BinaryOpExpression(std::unique_ptr<ExpressionNode> left, Token op, std::unique_ptr<ExpressionNode> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    void accept(ExpressionVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<ExpressionNode> clone() const override {
        return std::make_unique<BinaryOpExpression>(left->clone(), op, right->clone());
    }

    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;
};

} // namespace CHTL
