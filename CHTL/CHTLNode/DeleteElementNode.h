#pragma once

#include "BaseNode.h"
#include <string>

// Represents a 'delete element' instruction, e.g., `delete span;` or `delete div[1];`
struct DeleteElementNode : public BaseNode {
    std::string tagName;
    int index; // -1 if no index is specified

    explicit DeleteElementNode(const std::string& tagName, int index = -1)
        : BaseNode(NodeType::DeleteElement), tagName(tagName), index(index) {}
};
