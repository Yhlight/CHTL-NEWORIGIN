#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <iostream> // For debugging

namespace CHTL {

ExpressionEvaluator::ExpressionEvaluator() {}

EvaluatedValue ExpressionEvaluator::evaluate(const Expr& expr) {
    return evaluateAny(expr.accept(*this));
}

EvaluatedValue ExpressionEvaluator::evaluateAny(const std::any& anyValue) {
    try {
        return std::any_cast<EvaluatedValue>(anyValue);
    } catch (const std::bad_any_cast& e) {
        return EvaluatedValue{}; // Return NIL on error
    }
}

std::any ExpressionEvaluator::visitLiteralExpr(const LiteralExpr& expr) {
    switch (expr.value.type) {
        case TokenType::NUMBER_LITERAL:
            return EvaluatedValue{EvaluatedValue::Type::NUMBER, std::stod(expr.value.value)};
        case TokenType::STRING_LITERAL:
            return EvaluatedValue{EvaluatedValue::Type::STRING, 0.0, expr.value.value};
        case TokenType::UNQUOTED_LITERAL:
            return EvaluatedValue{EvaluatedValue::Type::STRING, 0.0, expr.value.value};
        default:
            return EvaluatedValue{}; // NIL
    }
}

std::any ExpressionEvaluator::visitGroupingExpr(const GroupingExpr& expr) {
    return evaluate(*expr.expression);
}

std::any ExpressionEvaluator::visitUnaryExpr(const UnaryExpr& expr) {
    EvaluatedValue right = evaluate(*expr.right);
    if (expr.op.type == TokenType::MINUS) {
        if (right.type == EvaluatedValue::Type::NUMBER) {
            return EvaluatedValue{EvaluatedValue::Type::NUMBER, -right.number_val};
        }
    }
    return EvaluatedValue{}; // NIL
}

std::any ExpressionEvaluator::visitTernaryExpr(const TernaryExpr& expr) {
    EvaluatedValue condition = evaluate(*expr.condition);
    if (condition.isTruthy()) {
        return evaluate(*expr.thenBranch);
    } else {
        return evaluate(*expr.elseBranch);
    }
}

std::any ExpressionEvaluator::visitBinaryExpr(const BinaryExpr& expr) {
    EvaluatedValue left = evaluate(*expr.left);
    EvaluatedValue right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            return EvaluatedValue{EvaluatedValue::Type::NUMBER, left.number_val - right.number_val};
        case TokenType::PLUS:
            if (left.type == EvaluatedValue::Type::NUMBER && right.type == EvaluatedValue::Type::NUMBER) {
                return EvaluatedValue{EvaluatedValue::Type::NUMBER, left.number_val + right.number_val};
            }
            if (left.type == EvaluatedValue::Type::STRING && right.type == EvaluatedValue::Type::STRING) {
                return EvaluatedValue{EvaluatedValue::Type::STRING, 0.0, left.string_val + right.string_val};
            }
            throw std::runtime_error("Operands must be two numbers or two strings for '+'.");
        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
             if (right.number_val == 0) throw std::runtime_error("Division by zero.");
            return EvaluatedValue{EvaluatedValue::Type::NUMBER, left.number_val / right.number_val};
        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            return EvaluatedValue{EvaluatedValue::Type::NUMBER, left.number_val * right.number_val};
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            return EvaluatedValue{EvaluatedValue::Type::BOOL, 0.0, "", left.number_val > right.number_val};
        case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            return EvaluatedValue{EvaluatedValue::Type::BOOL, 0.0, "", left.number_val < right.number_val};
        default:
            break;
    }

    return EvaluatedValue{};
}

void ExpressionEvaluator::checkNumberOperands(const Token& op, const EvaluatedValue& left, const EvaluatedValue& right) {
    if (left.type == EvaluatedValue::Type::NUMBER && right.type == EvaluatedValue::Type::NUMBER) return;
    throw std::runtime_error("Operands must be numbers for operator " + op.value);
}

} // namespace CHTL