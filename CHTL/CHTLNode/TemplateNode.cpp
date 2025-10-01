#include "TemplateNode.h"
#include <iostream>

namespace CHTL {

void TemplateNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Template: " << name << " (";
    switch (templateType) {
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

std::unique_ptr<BaseNode> TemplateNode::clone() const {
    auto clonedNode = std::make_unique<TemplateNode>(templateType, name);
    for (const auto& child : children) {
        if (child) {
            clonedNode->addChild(child->clone());
        }
    }
    return clonedNode;
}

} // namespace CHTL