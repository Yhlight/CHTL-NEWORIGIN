#include "StyleNode.h"
#include <iostream>

namespace CHTL {

void StyleNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "style {" << std::endl;

    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

std::unique_ptr<BaseNode> StyleNode::clone() const {
    auto clonedNode = std::make_unique<StyleNode>();
    for (const auto& prop : properties) {
        clonedNode->addProperty(std::unique_ptr<StylePropertyNode>(static_cast<StylePropertyNode*>(prop->clone().release())));
    }
    return clonedNode;
}

} // namespace CHTL