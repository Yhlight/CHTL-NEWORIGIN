#pragma once

#include "BaseNode.h"
#include <string>

class CustomUsageNode : public BaseNode {
public:
    CustomUsageNode(const std::string& type, const std::string& name)
        : customType(type), customName(name) {}

    NodeType getType() const override { return NodeType::CustomUsage; }

    const std::string& getCustomType() const { return customType; }
    const std::string& getCustomName() const { return customName; }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<CustomUsageNode>(customType, customName);
        for (const auto& child : getChildren()) {
            node->addChild(child->clone());
        }
        return node;
    }

private:
    std::string customType; // e.g., "Style"
    std::string customName;
};