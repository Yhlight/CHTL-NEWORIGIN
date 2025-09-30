#include "BinaryOpNode.h"
#include <iostream>

namespace CHTL {

BinaryOpNode::BinaryOpNode(std::unique_ptr<ExpressionNode> left, const Token& op, std::unique_ptr<ExpressionNode> right)
    : left(std::move(left)), op(op), right(std::move(right)) {}

void BinaryOpNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "BinaryOp: " << op.value << std::endl;

    if (left) {
        left->print(indent + 1);
    }
    if (right) {
        right->print(indent + 1);
    }
}

ExpressionType BinaryOpNode::getType() const {
    return ExpressionType::BINARY_OP;
}

const ExpressionNode* BinaryOpNode::getLeft() const {
    return left.get();
}

const Token& BinaryOpNode::getOperator() const {
    return op;
}

const ExpressionNode* BinaryOpNode::getRight() const {
    return right.get();
}

} // namespace CHTL