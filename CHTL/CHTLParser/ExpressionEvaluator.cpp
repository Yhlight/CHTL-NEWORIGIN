#include "ExpressionEvaluator.h"
#include <stdexcept>

double ExpressionEvaluator::evaluate(Expr& expr) {
    return expr.accept(*this);
}

double ExpressionEvaluator::visit(Binary& expr) {
    double left = evaluate(*expr.left);
    double right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::PLUS:
            return left + right;
        case TokenType::MINUS:
            return left - right;
        // Other operators will be added later
        default:
            throw std::runtime_error("Invalid binary operator.");
    }
}

double ExpressionEvaluator::visit(Literal& expr) {
    return std::stod(expr.value.lexeme);
}

double ExpressionEvaluator::visit(Grouping& expr) {
    return evaluate(*expr.expression);
}
