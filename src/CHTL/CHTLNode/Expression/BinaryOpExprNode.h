#pragma once

#include "ExprNode.h"
#include "../../CHTLLexer/Token.h"
#include "../../CHTLNode/AstVisitor.h"
#include <memory>

namespace CHTL {
    class BinaryOpExprNode : public ExprNode {
    public:
        std::unique_ptr<ExprNode> left;
        Token op;
        std::unique_ptr<ExprNode> right;

        BinaryOpExprNode(std::unique_ptr<ExprNode> left, const Token& op, std::unique_ptr<ExprNode> right)
            : left(std::move(left)), op(op), right(std::move(right)) {}

        void accept(AstVisitor &visitor) override {
            visitor.visit(*this);
        }
    };
}
