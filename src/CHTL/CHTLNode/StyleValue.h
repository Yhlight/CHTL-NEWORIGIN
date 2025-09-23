#pragma once

#include <string>

// Represents the value of a CSS property.
// A value can be a numeric quantity with a unit (e.g., 10px), which
// can be used in arithmetic expressions, or a simple string literal
// (e.g., "red", "solid", "auto").
struct StyleValue {
    enum ValueType { NUMERIC, STRING, EMPTY } type = EMPTY;

    // For NUMERIC types
    double numeric_val = 0.0;
    std::string unit;

    // For STRING types
    std::string string_val;
};
