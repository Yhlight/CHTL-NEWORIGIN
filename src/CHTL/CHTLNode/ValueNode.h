#pragma once

#include "ExpressionNode.h"
#include <string>

class ValueNode : public ExpressionNode {
public:
    ValueNode(const std::string& value) : value(value) {}

    NodeType getType() const override { return NodeType::Value; }

    const std::string& getValue() const {
        return value;
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ValueNode>(value);
    }

private:
    std::string value;
};