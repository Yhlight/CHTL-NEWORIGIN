#pragma once

#include "BaseNode.h"

class RootNode : public BaseNode {
public:
    RootNode() = default;

    NodeType getType() const override { return NodeType::Root; }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<RootNode>();
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }
};