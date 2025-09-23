#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <iostream>

class NamespaceNode : public BaseNode {
public:
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> children;

    explicit NamespaceNode(const std::string& name) : name(name) {}

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(child);
    }

    NodeType getType() const override { return NodeType::Namespace; }

    virtual void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "NamespaceNode (" << name << ")" << std::endl;
        for (const auto& child : children) {
            child->print(level + 1);
        }
    }
};
