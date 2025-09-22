#pragma once

#include "Expression.h"
#include "CHTL/Lexer/Token.h"
#include <memory>

class LogicalExpr : public Expr {
public:
    LogicalExpr(ExprPtr left, Token op, ExprPtr right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    Value accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const ExprPtr left;
    const Token op;
    const ExprPtr right;
};
