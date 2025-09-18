#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <cctype>

namespace CHTL {

// A simple helper to separate the numeric part and the unit part of a value.
static std::pair<double, std::string> extractValueAndUnit(const std::string& s) {
    size_t i = 0;
    // Read the numeric part (handles floating point numbers)
    while (i < s.length() && (isdigit(s[i]) || s[i] == '.')) {
        i++;
    }
    try {
        double value = std::stod(s.substr(0, i));
        std::string unit = s.substr(i);
        return {value, unit};
    } catch (const std::invalid_argument& e) {
        // If it's not a number (e.g., "red", "auto"), return 0 and the original string.
        return {0.0, s};
    }
}

std::string ExpressionEvaluator::evaluate(ExpressionNode& root) {
    m_result = "";
    root.accept(*this);
    return m_result;
}

void ExpressionEvaluator::visit(LiteralExpression& node) {
    m_result = node.token.value;
}

void ExpressionEvaluator::visit(BinaryOpExpression& node) {
    // Recursively evaluate left and right children
    std::string left_val_str = evaluate(*node.left);
    std::string right_val_str = evaluate(*node.right);

    auto left_pair = extractValueAndUnit(left_val_str);
    auto right_pair = extractValueAndUnit(right_val_str);

    double result_val = 0.0;

    // For now, we assume units are compatible if they are the same or one is empty.
    // A more robust system would handle conversions.
    if (!left_pair.second.empty() && !right_pair.second.empty() && left_pair.second != right_pair.second) {
         throw std::runtime_error("Mismatched units in style expression: " + left_pair.second + " and " + right_pair.second);
    }

    switch (node.op.type) {
        case TokenType::PLUS:
            result_val = left_pair.first + right_pair.first;
            break;
        case TokenType::MINUS:
            result_val = left_pair.first - right_pair.first;
            break;
        case TokenType::STAR:
            result_val = left_pair.first * right_pair.first;
            break;
        case TokenType::SLASH:
            if (right_pair.first == 0) throw std::runtime_error("Division by zero in style expression.");
            result_val = left_pair.first / right_pair.first;
            break;
        default:
            throw std::runtime_error("Unsupported operator in style expression: " + node.op.value);
    }

    // Use the unit from the left operand (or right if left is unitless)
    std::string result_unit = !left_pair.second.empty() ? left_pair.second : right_pair.second;

    // Convert double to string, removing trailing zeros for clean output
    std::string result_str = std::to_string(result_val);
    result_str.erase(result_str.find_last_not_of('0') + 1, std::string::npos);
    if (result_str.back() == '.') {
        result_str.pop_back();
    }

    m_result = result_str + result_unit;
}

} // namespace CHTL
