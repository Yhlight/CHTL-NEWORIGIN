#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> children;

    explicit NamespaceNode(std::string name)
        : name(std::move(name)) {}

    NodeType getType() const override {
        return NodeType::Namespace;
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }
};