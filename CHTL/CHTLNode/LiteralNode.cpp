#include "LiteralNode.h"
#include <iostream>

namespace CHTL {

LiteralNode::LiteralNode(const Token& value) : value(value) {}

void LiteralNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Literal: " << value.value << std::endl;
}

ExpressionType LiteralNode::getType() const {
    return ExpressionType::LITERAL;
}

const Token& LiteralNode::getValue() const {
    return value;
}

} // namespace CHTL