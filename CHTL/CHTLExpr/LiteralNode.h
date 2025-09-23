#pragma once

#include "BaseExprNode.h"
#include <string>

// Helper struct to hold a parsed numeric value and its unit
struct ParsedValue {
    double value;
    std::string unit;
};

// Represents a literal value in an expression.
// This could be a number with a unit ('100px'), a color ('red'),
// or a plain string ('solid').
#include <algorithm> // For std::find_if

// Helper to trim from start (in place)
static inline void ltrim_expr(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Helper to trim from end (in place)
static inline void rtrim_expr(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Represents a literal value in an expression.
// This could be a number with a unit ('100px'), a color ('red'),
// or a plain string ('solid').
struct LiteralNode : public BaseExprNode {
    std::string rawValue;

    // Optional: for evaluator convenience, we can pre-parse numeric values
    bool isNumeric = false;
    ParsedValue numericValue;

    explicit LiteralNode(const std::string& raw) : rawValue(raw) {
        // A simple parser to separate number from unit
        size_t first_char = raw.find_first_not_of("-.0123456789");
        if (first_char != std::string::npos) {
            try {
                numericValue.value = std::stod(raw.substr(0, first_char));
                numericValue.unit = raw.substr(first_char);
                // Trim whitespace from the extracted unit
                ltrim_expr(numericValue.unit);
                rtrim_expr(numericValue.unit);
                isNumeric = true;
            } catch (...) {
                // Not a numeric value
                isNumeric = false;
            }
        } else {
             try {
                numericValue.value = std::stod(raw);
                numericValue.unit = "";
                isNumeric = true;
            } catch (...) {
                isNumeric = false;
            }
        }
    }

    ExprNodeType getType() const override { return ExprNodeType::Literal; }
};
