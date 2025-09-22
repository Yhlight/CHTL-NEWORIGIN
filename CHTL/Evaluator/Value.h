#pragma once

#include <string>
#include <variant>
#include <vector>

// An evaluated value can be a number with a unit, a string, a boolean, or a color.
// For now, let's keep it simple. A value has a numeric part and a unit/string part.
// We can use a variant for more complex types later.

enum class ValueType {
    NUMBER,
    STRING,
    BOOLEAN,
    NIL
};

struct Value {
    ValueType type = ValueType::NIL;
    double number = 0.0;
    std::string string; // Also used for units like 'px', 'em'
    bool boolean = false;

    // Helper to convert back to a CSS string
    std::string to_css_string() const {
        if (type == ValueType::NUMBER) {
            // to_string would add trailing zeros, which is not ideal for CSS.
            std::string s = std::to_string(number);
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            s.erase(s.find_last_not_of('.') + 1, std::string::npos);
            return s + string; // string here is the unit
        }
        if (type == ValueType::STRING) {
            return string;
        }
        return ""; // Should not happen for valid CSS
    }
};
