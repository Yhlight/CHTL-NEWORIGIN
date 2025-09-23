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

    NodeType getType() const override { return NodeType::InsertElement; }
};
