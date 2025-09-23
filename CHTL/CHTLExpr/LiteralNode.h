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
    std::shared_ptr<BaseExprNode> clone() const override {
        return std::make_shared<LiteralNode>(rawValue);
    }
};
