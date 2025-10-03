#pragma once

#include "BaseNode.h"
#include <string>

class ClassSelectorNode : public BaseNode {
public:
    ClassSelectorNode(const std::string& name) : name(name) {}

    NodeType getType() const override { return NodeType::ClassSelector; }

    const std::string& getName() const {
        return name;
    }

private:
    std::string name;
};