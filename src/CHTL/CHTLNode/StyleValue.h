#pragma once

#include <string>
#include <memory>

// Forward declaration
struct StyleValue;

// Represents a dynamic conditional expression, e.g. {{box}}->width > 2 ? 100px : 50px
struct DynamicConditionalExpression {
    std::string selector;
    std::string property;
    std::string op;
    double value_to_compare;
    std::unique_ptr<StyleValue> true_branch;
    std::unique_ptr<StyleValue> false_branch;

    DynamicConditionalExpression() = default;
    DynamicConditionalExpression(const DynamicConditionalExpression& other);
    DynamicConditionalExpression& operator=(const DynamicConditionalExpression& other);
};

// Represents the value of a CSS property or an intermediate value in an expression.
// Can hold a numeric value, a string, a boolean, or be empty.
struct StyleValue {
    enum ValueType { NUMERIC, STRING, BOOL, EMPTY, DELETED, RESPONSIVE, DYNAMIC_CONDITIONAL } type = EMPTY;

    // For NUMERIC types
    double numeric_val = 0.0;
    std::string unit;

    // For STRING types
    std::string string_val;

    // For BOOL types
    bool bool_val = false;

    // For RESPONSIVE types
    std::string responsive_var_name;

    // For DYNAMIC_CONDITIONAL types
    std::shared_ptr<DynamicConditionalExpression> dynamic_expr;

    StyleValue() = default;
    StyleValue(const StyleValue& other);
    StyleValue& operator=(const StyleValue& other);
};
