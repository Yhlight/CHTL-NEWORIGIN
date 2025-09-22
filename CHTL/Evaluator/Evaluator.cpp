#include "Evaluator.h"
#include <stdexcept>
#include <cmath>

// Expression headers
#include "CHTL/Expression/BinaryExpr.h"
#include "CHTL/Expression/UnaryExpr.h"
#include "CHTL/Expression/LiteralExpr.h"
#include "CHTL/Expression/ReferenceExpr.h"
#include "CHTL/Expression/ConditionalExpr.h"
#include "CHTL/Expression/LogicalExpr.h"

// Helper to parse a string like "100px" into a number and a unit
Value parse_literal_value(const std::string& literal) {
    if (literal.empty()) return {ValueType::NIL};

    // Check if it's a number-based value
    size_t i = 0;
    if (literal[0] == '-') i++;
    if (std::isdigit(literal[i])) {
        try {
            size_t first_non_digit;
            double number = std::stod(literal, &first_non_digit);
            std::string unit = literal.substr(first_non_digit);
            return {ValueType::NUMBER, number, unit, false};
        } catch (const std::invalid_argument&) {
            // Not a number, treat as a string
        }
    }

    // Otherwise, it's a string (e.g., "red", "bold")
    return {ValueType::STRING, 0.0, literal, false};
}

Evaluator::Evaluator(const NodeList& ast_root, const ElementNode& current_element)
    : ast_root(ast_root), current_element(current_element) {}

Value Evaluator::evaluate(const Expr& expr) {
    return expr.accept(*this);
}

bool Evaluator::is_truthy(const Value& value) const {
    switch (value.type) {
        case ValueType::NIL: return false;
        case ValueType::BOOLEAN: return value.boolean;
        case ValueType::NUMBER: return value.number != 0;
        case ValueType::STRING: return !value.string.empty();
        default: return false;
    }
}

Value Evaluator::visit(const LiteralExpr& expr) {
    return parse_literal_value(expr.value);
}

Value Evaluator::visit(const UnaryExpr& expr) {
    Value right = evaluate(*expr.right);
    if (expr.op.type == TokenType::MINUS) {
        if (right.type == ValueType::NUMBER) {
            right.number = -right.number;
            return right;
        }
    }
    return {ValueType::NIL};
}

#include <iostream>

Value Evaluator::visit(const BinaryExpr& expr) {
    Value left = evaluate(*expr.left);
    Value right = evaluate(*expr.right);

    std::cout << "DEBUG: BinaryExpr op='" << expr.op.lexeme << "' left=" << (int)left.type << ", right=" << (int)right.type << std::endl;

    if (left.type == ValueType::NUMBER && right.type == ValueType::NUMBER) {
        std::string result_unit = left.string;
        if (result_unit.empty()) {
            result_unit = right.string;
        } else if (!right.string.empty() && left.string != right.string) {
            throw std::runtime_error("Mismatched units in binary operation: " + left.string + " and " + right.string);
        }
        Value result{ValueType::NUMBER, 0.0, result_unit, false};
        switch (expr.op.type) {
            case TokenType::PLUS: result.number = left.number + right.number; return result;
            case TokenType::MINUS: result.number = left.number - right.number; return result;
            case TokenType::STAR: result.number = left.number * right.number; return result;
            case TokenType::SLASH: result.number = left.number / right.number; return result;
            case TokenType::PERCENT: result.number = fmod(left.number, right.number); return result;
            case TokenType::GREATER: return {ValueType::BOOLEAN, 0.0, "", left.number > right.number};
            case TokenType::GREATER_EQUAL: return {ValueType::BOOLEAN, 0.0, "", left.number >= right.number};
            case TokenType::LESS: return {ValueType::BOOLEAN, 0.0, "", left.number < right.number};
            case TokenType::LESS_EQUAL: return {ValueType::BOOLEAN, 0.0, "", left.number <= right.number};
            case TokenType::EQUAL_EQUAL: return {ValueType::BOOLEAN, 0.0, "", left.number == right.number};
            case TokenType::BANG_EQUAL: return {ValueType::BOOLEAN, 0.0, "", left.number != right.number};
            default: break;
        }
    }

    if (left.type == ValueType::STRING && right.type == ValueType::STRING) {
        if (expr.op.type == TokenType::EQUAL_EQUAL) {
            return {ValueType::BOOLEAN, 0.0, "", left.string == right.string};
        }
        if (expr.op.type == TokenType::BANG_EQUAL) {
            return {ValueType::BOOLEAN, 0.0, "", left.string != right.string};
        }
    }

    throw std::runtime_error("Unsupported operands for binary operation.");
}

Value Evaluator::visit(const LogicalExpr& expr) {
    Value left = evaluate(*expr.left);
    if (expr.op.type == TokenType::LOGICAL_OR) {
        if (is_truthy(left)) return left;
    } else { // LOGICAL_AND
        if (!is_truthy(left)) return left;
    }
    return evaluate(*expr.right);
}

Value Evaluator::visit(const ConditionalExpr& expr) {
    Value condition = evaluate(*expr.condition);
    if (is_truthy(condition)) {
        return evaluate(*expr.then_branch);
    } else if (expr.else_branch) {
        return evaluate(*expr.else_branch);
    }
    return {ValueType::NIL};
}

Value Evaluator::visit(const ReferenceExpr& expr) {
    // Simplified: only handle single-identifier paths for now
    if (expr.path.size() == 1 && expr.path[0].type == TokenType::IDENTIFIER) {
        std::string key = expr.path[0].lexeme;
        // Search for property on current element
        for (const auto& prop : current_element.styles) {
            if (prop.key == key) {
                return evaluate(*prop.value);
            }
        }
        // If not found as a reference, treat it as a string literal
        return {ValueType::STRING, 0.0, key, false};
    }

    // Full implementation would search the AST based on a selector path.
    return {ValueType::NIL};
}
