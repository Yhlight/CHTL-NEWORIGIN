#include "StyleNode.h"
#include <iostream>

namespace CHTL {

void StyleNode::addProperty(std::unique_ptr<StylePropertyNode> property) {
    properties.push_back(std::move(property));
}

void StyleNode::addSelectorBlock(std::unique_ptr<SelectorBlockNode> selectorBlock) {
    selector_blocks.push_back(std::move(selectorBlock));
}

const std::vector<std::unique_ptr<StylePropertyNode>>& StyleNode::getProperties() const {
    return properties;
}

const std::vector<std::unique_ptr<SelectorBlockNode>>& StyleNode::getSelectorBlocks() const {
    return selector_blocks;
}

void StyleNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "style {" << std::endl;

    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }

    for (const auto& block : selector_blocks) {
        block->print(indent + 1);
    }

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

} // namespace CHTL