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

    void setValue(std::unique_ptr<ExpressionNode> newValue) {
        value = std::move(newValue);
    }

private:
    std::string name;
    std::unique_ptr<ExpressionNode> value;

public:
    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<StylePropertyNode>(name, std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(value->clone().release())));
    }
};