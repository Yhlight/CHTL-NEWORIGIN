#include "ExpressionEvaluator.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/LiteralNode.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

Value ExpressionEvaluator::evaluate(ExpressionNode* root) {
    if (!root) return Value::make_string("");
    return root->accept(*this);
}

Value ExpressionEvaluator::visit(LiteralNode& node) {
    const std::string& lexeme = node.value.lexeme;

    // Try to parse as a number with an optional unit
    try {
        size_t i = 0;
        // A slightly more robust check for the numeric part
        if (lexeme.length() > 0 && (lexeme[0] == '-' || lexeme[0] == '+')) i++;
        while (i < lexeme.length() && (isdigit(lexeme[i]) || lexeme[i] == '.')) {
            i++;
        }

        // If the whole string was parsed as a number part, it's a number.
        if (i > 0 && i <= lexeme.length()) {
            std::string num_part = lexeme.substr(0, i);
            std::string unit_part = lexeme.substr(i);
            double num = std::stod(num_part);
            return Value::make_number(num, unit_part);
        }
    } catch (...) {
        // Fall through to treat as a string if stod fails
    }

    // If it's not a valid number-unit format, treat it as a plain string
    return Value::make_string(lexeme);
}

Value ExpressionEvaluator::visit(BinaryOpNode& node) {
    Value left_val = node.left->accept(*this);
    Value right_val = node.right->accept(*this);

    if (left_val.type != ValueType::NUMBER || right_val.type != ValueType::NUMBER) {
        throw std::runtime_error("Arithmetic operations can only be performed on numeric values.");
    }

    // Simple unit handling: use the unit from the left operand.
    std::string result_unit = left_val.unit;

    switch (node.op.type) {
        case TokenType::PLUS:
            return Value::make_number(left_val.number_val + right_val.number_val, result_unit);
        case TokenType::MINUS:
            return Value::make_number(left_val.number_val - right_val.number_val, result_unit);
        case TokenType::ASTERISK:
            return Value::make_number(left_val.number_val * right_val.number_val, result_unit);
        case TokenType::SLASH:
            if (right_val.number_val == 0) throw std::runtime_error("Division by zero.");
            return Value::make_number(left_val.number_val / right_val.number_val, result_unit);
        default:
            throw std::runtime_error("Unknown binary operator.");
    }
}

} // namespace CHTL
