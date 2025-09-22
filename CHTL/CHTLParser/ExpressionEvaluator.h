#pragma once

#include "Expression.h"
#include <string>

class ExpressionEvaluator : public ExprVisitor {
public:
    double evaluate(Expr& expr);

    double visit(Binary& expr) override;
    double visit(Literal& expr) override;
    double visit(Grouping& expr) override;
};
