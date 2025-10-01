#include "StylePropertyNode.h"
#include <iostream>

namespace CHTL {

void StylePropertyNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << key << ":" << std::endl;
    if (value) {
        value->print(indent + 1);
    }
}

const std::string& StylePropertyNode::getKey() const {
    return key;
}

const ExpressionNode* StylePropertyNode::getValue() const {
    return value.get();
}

std::unique_ptr<BaseNode> StylePropertyNode::clone() const {
    auto cloned_value = value ? std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(value->clone().release())) : nullptr;
    return std::make_unique<StylePropertyNode>(key, std::move(cloned_value));
}

} // namespace CHTL