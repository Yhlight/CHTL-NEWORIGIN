#pragma once

#include "BaseNode.h"
#include <string>

class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& name, const std::string& value)
        : name(name), value(value) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::string name;
    std::string value;
};
