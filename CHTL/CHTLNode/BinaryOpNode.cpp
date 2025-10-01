#include "BinaryOpNode.h"
#include <iostream>

namespace CHTL {

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

const ExpressionNode* BinaryOpNode::getLeft() const {
    return left.get();
}

const Token& BinaryOpNode::getOperator() const {
    return op;
}

const ExpressionNode* BinaryOpNode::getRight() const {
    return right.get();
}

std::unique_ptr<BaseNode> BinaryOpNode::clone() const {
    auto cloned_left = left ? std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(left->clone().release())) : nullptr;
    auto cloned_right = right ? std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(right->clone().release())) : nullptr;
    return std::make_unique<BinaryOpNode>(std::move(cloned_left), op, std::move(cloned_right));
}

} // namespace CHTL