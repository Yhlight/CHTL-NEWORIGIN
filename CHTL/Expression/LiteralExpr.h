#pragma once

#include "Expression.h"
#include <string>

class LiteralExpr : public Expr {
public:
    explicit LiteralExpr(std::string value) : value(std::move(value)) {}

    Value accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::string value;
};
