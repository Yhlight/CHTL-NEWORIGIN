#pragma once

#include "ExprNode.h"
#include "../../CHTLLexer/Token.h" // For storing the literal token
#include "../../CHTLNode/AstVisitor.h"
#include <string>

namespace CHTL {
    class LiteralExprNode : public ExprNode {
    public:
        Token literal;

        explicit LiteralExprNode(const Token& token) : literal(token) {}

        void accept(AstVisitor &visitor) override {
            visitor.visit(*this);
        }
    };
}
