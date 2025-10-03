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

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ClassSelectorNode>(name);
    }

private:
    std::string name;
};