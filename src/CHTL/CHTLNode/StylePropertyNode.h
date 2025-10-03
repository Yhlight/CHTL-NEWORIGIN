#pragma once

#include "BaseNode.h"
#include "ExpressionNode.h"
#include <string>
#include <memory>

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& name, std::unique_ptr<ExpressionNode> value)
        : name(name), value(std::move(value)) {}

    NodeType getType() const override { return NodeType::StyleProperty; }

    const std::string& getName() const { return name; }
    const ExpressionNode* getValue() const { return value.get(); }

private:
    std::string name;
    std::unique_ptr<ExpressionNode> value;
};