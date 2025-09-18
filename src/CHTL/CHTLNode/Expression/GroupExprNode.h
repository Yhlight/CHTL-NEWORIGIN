#pragma once

#include "ExprNode.h"
#include "../../CHTLNode/AstVisitor.h"
#include <memory>

namespace CHTL {
    class GroupExprNode : public ExprNode {
    public:
        std::unique_ptr<ExprNode> expression;

        explicit GroupExprNode(std::unique_ptr<ExprNode> expression)
            : expression(std::move(expression)) {}

        void accept(AstVisitor &visitor) override {
            visitor.visit(*this);
        }
    };
}
