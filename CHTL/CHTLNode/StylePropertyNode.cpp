#include "StylePropertyNode.h"
#include <iostream>

namespace CHTL {

StylePropertyNode::StylePropertyNode(const std::string& key, std::unique_ptr<ExpressionNode> value)
    : key(key), value(std::move(value)) {}

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

} // namespace CHTL