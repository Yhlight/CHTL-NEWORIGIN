#pragma once

#include "ExpressionBaseNode.h"
#include <string>

// Represents a literal value in an expression, such as a number ("100px") or a string ("red").
class LiteralNode : public ExpressionBaseNode {
public:
    std::string value;

    explicit LiteralNode(const std::string& val) : value(val) {}

    ExpressionNodeType getType() const override {
        return ExpressionNodeType::Literal;
    }
};