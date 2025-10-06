#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"
#include <memory>

namespace CHTL {
    class UnaryExpressionNode : public ExpressionNode {
    public:
        Token op;
        std::shared_ptr<ExpressionNode> right;

        UnaryExpressionNode(const Token& op, std::shared_ptr<ExpressionNode> right)
            : ExpressionNode(NodeType::NODE_EXPRESSION_UNARY), op(op), right(right) {}
    };
}