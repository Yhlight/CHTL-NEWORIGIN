#pragma once

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"
#include <memory>

namespace CHTL {
    class BinaryExpressionNode : public ExpressionNode {
    public:
        std::shared_ptr<ExpressionNode> left;
        Token op;
        std::shared_ptr<ExpressionNode> right;

        BinaryExpressionNode(std::shared_ptr<ExpressionNode> left, const Token& op, std::shared_ptr<ExpressionNode> right)
            : ExpressionNode(NodeType::NODE_EXPRESSION_BINARY), left(left), op(op), right(right) {}
    };
}