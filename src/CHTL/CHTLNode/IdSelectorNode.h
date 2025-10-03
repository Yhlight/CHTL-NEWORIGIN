#pragma once

#include "BaseNode.h"
#include <string>

class IdSelectorNode : public BaseNode {
public:
    IdSelectorNode(const std::string& name) : name(name) {}

    NodeType getType() const override { return NodeType::IdSelector; }

    const std::string& getName() const {
        return name;
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<IdSelectorNode>(name);
    }

private:
    std::string name;
};