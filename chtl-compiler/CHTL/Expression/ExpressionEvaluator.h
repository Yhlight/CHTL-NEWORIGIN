#pragma once

#include <string>
#include <stdexcept>
#include <regex>

class ExpressionEvaluator {
public:
    static std::string evaluate(const std::string& left, const std::string& op, const std::string& right) {
        std::regex re("(-?[0-9]+\\.?[0-9]*)(.*)");
        std::smatch left_match, right_match;

        if (!std::regex_match(left, left_match, re) || !std::regex_match(right, right_match, re)) {
            throw std::runtime_error("Invalid operands for expression: " + left + op + right);
        }

        double left_val = std::stod(left_match[1].str());
        std::string left_unit = left_match[2].str();
        double right_val = std::stod(right_match[1].str());
        std::string right_unit = right_match[2].str();

        // For simplicity, units must match or one must be empty.
        if (!left_unit.empty() && !right_unit.empty() && left_unit != right_unit) {
            throw std::runtime_error("Mismatched units in expression: " + left_unit + " and " + right_unit);
        }

        std::string result_unit = !left_unit.empty() ? left_unit : right_unit;
        double result_val = 0;

        if (op == "+") {
            result_val = left_val + right_val;
        } else if (op == "-") {
            result_val = left_val - right_val;
        } else if (op == "*") {
            result_val = left_val * right_val;
        } else if (op == "/") {
            if (right_val == 0) throw std::runtime_error("Division by zero.");
            result_val = left_val / right_val;
        } else {
            throw std::runtime_error("Unsupported operator: " + op);
        }

        std::string result_str = std::to_string(result_val);
        // Remove trailing zeros
        result_str.erase(result_str.find_last_not_of('0') + 1, std::string::npos);
        if (result_str.back() == '.') {
            result_str.pop_back();
        }

        return result_str + result_unit;
    }
};