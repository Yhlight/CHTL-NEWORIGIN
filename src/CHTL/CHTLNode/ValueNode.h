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

private:
    std::string value;
};