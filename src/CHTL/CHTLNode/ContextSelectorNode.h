#pragma once

#include "BaseNode.h"
#include <string>

class ContextSelectorNode : public BaseNode {
public:
    ContextSelectorNode(const std::string& selector) : selector(selector) {}

    NodeType getType() const override { return NodeType::ContextSelector; }

    const std::string& getSelector() const {
        return selector;
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<ContextSelectorNode>(selector);
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

private:
    std::string selector;
};