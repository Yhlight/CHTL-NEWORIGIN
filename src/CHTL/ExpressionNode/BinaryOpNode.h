#pragma once

#include "ExpressionBaseNode.h"
#include <string>
#include <memory>

// Represents a binary operation, e.g., "100px + 50px" or "width > 50".
class BinaryOpNode : public ExpressionBaseNode {
public:
    std::unique_ptr<ExpressionBaseNode> left;
    std::string op;
    std::unique_ptr<ExpressionBaseNode> right;

    BinaryOpNode(std::unique_ptr<ExpressionBaseNode> l, const std::string& o, std::unique_ptr<ExpressionBaseNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    ExpressionNodeType getType() const override {
        return ExpressionNodeType::BinaryOp;
    }

    std::unique_ptr<ExpressionBaseNode> clone() const override {
        return std::make_unique<BinaryOpNode>(left->clone(), op, right->clone());
    }
};