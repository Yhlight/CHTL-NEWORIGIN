#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// Represents a [Namespace] block
class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> children;

    explicit NamespaceNode(const std::string& name)
        : BaseNode(NodeType::Namespace), name(name) {}

    void addChild(const std::shared_ptr<BaseNode>& child) {
        children.push_back(child);
    }

    void print(int level = 0) const override {
        std::cout << std::string(level * 2, ' ') << "NamespaceNode: " << name << " {\n";
        for (const auto& child : children) {
            child->print(level + 1);
        }
        std::cout << std::string(level * 2, ' ') << "}\n";
    }
};
