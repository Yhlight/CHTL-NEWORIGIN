#include "LiteralNode.h"
#include <iostream>

namespace CHTL {

LiteralNode::LiteralNode(const Token& value, const std::string& unit) : value(value), unit(unit) {}

void LiteralNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Literal: " << value.value;
    if (!unit.empty()) {
        std::cout << " " << unit;
    }
    std::cout << std::endl;
}

ExpressionType LiteralNode::getType() const {
    return ExpressionType::LITERAL;
}

const Token& LiteralNode::getValue() const {
    return value;
}

const std::string& LiteralNode::getUnit() const {
    return unit;
}

} // namespace CHTL