#pragma once

#include "BaseNode.h"
#include <string>

// Represents a 'delete element' instruction, e.g., `delete span;`
struct DeleteElementNode : public BaseNode {
    std::string targetSelector; // e.g., "span", "div[1]"

    explicit DeleteElementNode(const std::string& selector)
        : BaseNode(NodeType::DeleteElement), targetSelector(selector) {}
};
