#include "InsertNode.h"
#include <iostream>

namespace CHTL {

void InsertNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Insert: ";
    switch (position) {
        case InsertPosition::AFTER: std::cout << "after"; break;
        case InsertPosition::BEFORE: std::cout << "before"; break;
        case InsertPosition::REPLACE: std::cout << "replace"; break;
        case InsertPosition::AT_TOP: std::cout << "at top"; break;
        case InsertPosition::AT_BOTTOM: std::cout << "at bottom"; break;
    }
    std::cout << " " << target << std::endl;

    for (const auto& child : children) {
        if (child) {
            child->print(indent + 1);
        }
    }
}

std::unique_ptr<BaseNode> InsertNode::clone() const {
    auto clonedNode = std::make_unique<InsertNode>(position, target);
    for (const auto& child : children) {
        if (child) {
            clonedNode->addChild(child->clone());
        }
    }
    return clonedNode;
}

} // namespace CHTL