#pragma once

#include "BaseExprNode.h"
#include "../CHTLLexer/Token.h" // For TokenType enum
#include <memory>

struct BinaryOpNode : public BaseExprNode {
    std::shared_ptr<BaseExprNode> left;
    TokenType op;
    std::shared_ptr<BaseExprNode> right;

    BinaryOpNode(std::shared_ptr<BaseExprNode> left, TokenType op, std::shared_ptr<BaseExprNode> right)
        : left(left), op(op), right(right) {}

    ExprNodeType getType() const override { return ExprNodeType::BinaryOp; }
};
