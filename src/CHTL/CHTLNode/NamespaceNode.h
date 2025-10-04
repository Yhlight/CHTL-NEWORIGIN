#pragma once

#include "BaseNode.h"
#include <string>

class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name) : name(name) {}

    NodeType getType() const override { return NodeType::Namespace; }

    const std::string& getName() const { return name; }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<NamespaceNode>(name);
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

private:
    std::string name;
};