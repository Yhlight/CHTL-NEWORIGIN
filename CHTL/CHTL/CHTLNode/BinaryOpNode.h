#pragma once

#include "ExpressionNode.h"
#include <memory>

namespace CHTL {

class BinaryOpNode : public ExpressionNode {
public:
    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;

    BinaryOpNode(std::unique_ptr<ExpressionNode> l, const Token& o, std::unique_ptr<ExpressionNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    Value accept(ExpressionVisitor& visitor) override {
        return visitor.visit(*this);
    }
};

} // namespace CHTL
