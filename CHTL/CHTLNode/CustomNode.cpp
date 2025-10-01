#include "CustomNode.h"
#include <iostream>

namespace CHTL {

void CustomNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Custom: " << name << " (";
    switch (customType) {
        case TemplateType::STYLE:
            std::cout << "Style";
            break;
        case TemplateType::ELEMENT:
            std::cout << "Element";
            break;
        case TemplateType::VAR:
            std::cout << "Var";
            break;
    }
    std::cout << ")" << std::endl;

    for (const auto& child : children) {
        if (child) {
            child->print(indent + 1);
        }
    }
}

std::unique_ptr<BaseNode> CustomNode::clone() const {
    auto clonedNode = std::make_unique<CustomNode>(customType, name);
    for (const auto& child : children) {
        if (child) {
            clonedNode->addChild(child->clone());
        }
    }
    return clonedNode;
}

} // namespace CHTL