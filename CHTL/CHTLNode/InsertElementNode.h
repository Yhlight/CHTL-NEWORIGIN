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
    std::string targetTagName;
    int targetIndex; // -1 if not specified
    std::vector<std::shared_ptr<BaseNode>> nodesToInsert;

    InsertElementNode() : BaseNode(NodeType::InsertElement), targetIndex(-1) {}
};
