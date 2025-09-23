#pragma once

#include "BaseNode.h"
#include <string>

// Represents a 'delete element' instruction, e.g., `delete span;`
struct DeleteElementNode : public BaseNode {
    std::string targetSelector; // e.g., "span", "div[1]"

    explicit DeleteElementNode(const std::string& selector) : targetSelector(selector) {}
    NodeType getType() const override { return NodeType::DeleteElement; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "DeleteElementNode (target: " << targetSelector << ")" << std::endl;
    }
};
