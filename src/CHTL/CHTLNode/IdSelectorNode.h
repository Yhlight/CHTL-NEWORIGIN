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
        auto clonedNode = std::make_unique<IdSelectorNode>(name);
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

private:
    std::string name;
};