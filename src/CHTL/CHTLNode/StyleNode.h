#pragma once

#include "BaseNode.h"
#include <string>

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    NodeType getType() const override { return NodeType::Style; }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<StyleNode>();
        for (const auto& child : getChildren()) {
            node->addChild(child->clone());
        }
        return node;
    }
};