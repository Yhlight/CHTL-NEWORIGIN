#pragma once

#include "Expression.h"
#include "CHTL/Lexer/Token.h"
#include <memory>

class UnaryExpr : public Expr {
public:
    UnaryExpr(Token op, ExprPtr right)
        : op(std::move(op)), right(std::move(right)) {}

    Value accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const Token op;
    const ExprPtr right;
};
