#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <cmath>

namespace CHTL {

EvaluatedValue ExpressionEvaluator::evaluate(const ExpressionNode* expression) {
    if (!expression) {
        throw std::runtime_error("Cannot evaluate a null expression.");
    }

    switch (expression->getType()) {
        case ExpressionType::LITERAL: {
            const auto* literal = static_cast<const LiteralNode*>(expression);
            if (literal->getValue().type == TokenType::NUMBER) {
                double value = 0.0;
                try {
                    value = std::stod(literal->getValue().value);
                } catch (const std::invalid_argument&) {
                    throw std::runtime_error("Invalid number in expression: " + literal->getValue().value);
                }
                return {EvaluatedValue::Type::NUMBER, value, "", literal->getUnit()};
            } else {
                return {EvaluatedValue::Type::STRING, 0.0, literal->getValue().value, ""};
            }
        }
        case ExpressionType::BINARY_OP: {
            const auto* binaryOp = static_cast<const BinaryOpNode*>(expression);
            EvaluatedValue left = evaluate(binaryOp->getLeft());
            EvaluatedValue right = evaluate(binaryOp->getRight());

            if (left.type == EvaluatedValue::Type::STRING || right.type == EvaluatedValue::Type::STRING) {
                throw std::runtime_error("Arithmetic operations are not supported on string literals.");
            }

            switch (binaryOp->getOperator().type) {
                case TokenType::PLUS:
                    if (left.unit.empty()) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value + right.number_value, "", right.unit};
                    } else if (right.unit.empty()) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value + right.number_value, "", left.unit};
                    } else if (left.unit == right.unit) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value + right.number_value, "", left.unit};
                    } else {
                        throw std::runtime_error("Incompatible units for addition: '" + left.unit + "' and '" + right.unit + "'");
                    }
                case TokenType::MINUS:
                    if (left.unit.empty() && !right.unit.empty()) {
                         throw std::runtime_error("Cannot subtract a value with unit from a unit-less value.");
                    } else if (right.unit.empty()) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value - right.number_value, "", left.unit};
                    } else if (left.unit == right.unit) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value - right.number_value, "", left.unit};
                    } else {
                        throw std::runtime_error("Incompatible units for subtraction: '" + left.unit + "' and '" + right.unit + "'");
                    }
                case TokenType::STAR:
                    if (!left.unit.empty() && !right.unit.empty()) {
                        throw std::runtime_error("Cannot multiply two values with units: '" + left.unit + "' and '" + right.unit + "'");
                    }
                    if (!left.unit.empty()) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value * right.number_value, "", left.unit};
                    } else {
                        return {EvaluatedValue::Type::NUMBER, left.number_value * right.number_value, "", right.unit};
                    }
                case TokenType::SLASH:
                    if (left.unit == right.unit && !left.unit.empty()) {
                        return {EvaluatedValue::Type::NUMBER, left.number_value / right.number_value, "", ""};
                    } else if (!right.unit.empty()) {
                        throw std::runtime_error("Cannot divide by a value with unit: '" + right.unit + "'");
                    } else {
                        return {EvaluatedValue::Type::NUMBER, left.number_value / right.number_value, "", left.unit};
                    }
                case TokenType::PERCENT: // Modulo
                     if (!left.unit.empty() || !right.unit.empty()) {
                        throw std::runtime_error("Modulo operator does not support units.");
                    }
                    return {EvaluatedValue::Type::NUMBER, std::fmod(left.number_value, right.number_value), "", ""};
                case TokenType::STAR_STAR: // Power
                    if (!left.unit.empty() || !right.unit.empty()) {
                        throw std::runtime_error("Power operator does not support units.");
                    }
                    return {EvaluatedValue::Type::NUMBER, std::pow(left.number_value, right.number_value), "", ""};
                default:
                    throw std::runtime_error("Unsupported binary operator.");
            }
        }
        default:
            throw std::runtime_error("Unsupported expression type.");
    }
}

} // namespace CHTL