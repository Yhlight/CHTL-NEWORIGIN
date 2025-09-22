#pragma once

#include "Expression.h"
#include <memory>

class ConditionalExpr : public Expr {
public:
    ConditionalExpr(ExprPtr condition, ExprPtr then_branch, ExprPtr else_branch)
        : condition(std::move(condition)),
          then_branch(std::move(then_branch)),
          else_branch(std::move(else_branch)) {}

    Value accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const ExprPtr condition;
    const ExprPtr then_branch;
    const ExprPtr else_branch; // Can be nullptr if it's an optional else
};
