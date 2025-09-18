#pragma once

#include "ExprNode.h"
#include "../../CHTLLexer/Token.h"
#include "../../CHTLNode/AstVisitor.h"
#include <memory>

namespace CHTL {
    class PropertyAccessExprNode : public ExprNode {
    public:
        // The object being accessed, e.g., 'box' in 'box.width'
        // This could be more complex later, so we use an ExprNode.
        std::unique_ptr<ExprNode> object;
        // The property being accessed, e.g., 'width'
        Token property;

        PropertyAccessExprNode(std::unique_ptr<ExprNode> object, const Token& property)
            : object(std::move(object)), property(property) {}

        void accept(AstVisitor &visitor) override {
            visitor.visit(*this);
        }
    };
}
