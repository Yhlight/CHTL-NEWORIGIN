#pragma once

#include "BaseNode.h"
#include <string>

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& name, const std::string& value) : name(name), value(value) {}

    NodeType getType() const override { return NodeType::StyleProperty; }

    const std::string& getName() const {
        return name;
    }

    const std::string& getValue() const {
        return value;
    }

private:
    std::string name;
    std::string value;
};