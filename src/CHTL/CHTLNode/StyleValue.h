#pragma once

#include <string>

// Represents the value of a CSS property or an intermediate value in an expression.
// Can hold a numeric value, a string, a boolean, or be empty.
struct StyleValue {
    enum ValueType { NUMERIC, STRING, BOOL, EMPTY, DELETED, RESPONSIVE } type = EMPTY;

    // For NUMERIC types
    double numeric_val = 0.0;
    std::string unit;

    // For STRING types
    std::string string_val;

    // For BOOL types
    bool bool_val = false;

    // For RESPONSIVE types
    std::string responsive_var_name;
};
