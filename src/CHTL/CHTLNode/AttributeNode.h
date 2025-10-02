#pragma once

#include "BaseNode.h"
#include <string>

class AttributeNode : public BaseNode {
public:
    AttributeNode(const std::string& name, const std::string& value)
        : name(name), value(value) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string name;
    std::string value;
};