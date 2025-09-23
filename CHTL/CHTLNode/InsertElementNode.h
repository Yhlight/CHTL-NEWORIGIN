#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

enum class InsertPosition {
    After,
    Before,
    Replace,
    AtTop,
    AtBottom
};

// Represents an 'insert' instruction.
struct InsertElementNode : public BaseNode {
    InsertPosition position;
    std::string targetSelector; // Can be empty for AtTop/AtBottom
    std::vector<std::shared_ptr<BaseNode>> nodesToInsert;

    InsertElementNode(InsertPosition pos, const std::string& selector)
        : position(pos), targetSelector(selector) {}

    NodeType getType() const override { return NodeType::InsertElement; }

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "InsertElementNode (selector: " << targetSelector << ")" << std::endl;
        for (const auto& node : nodesToInsert) {
            node->print(level + 1);
        }
    }
};
