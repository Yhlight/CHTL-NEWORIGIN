#pragma once

#include "Expression.h"
#include <string>
#include <vector>

class ReferenceExpr : public Expr {
public:
    // A reference is a chain of identifiers/selectors, e.g., ".box", "width"
    explicit ReferenceExpr(std::vector<Token> path) : path(std::move(path)) {}

    Value accept(ExprVisitor& visitor) const override {
        return visitor.visit(*this);
    }

    const std::vector<Token> path;
};
