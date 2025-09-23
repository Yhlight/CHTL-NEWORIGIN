#pragma once
#include "../CHTLNode/StyleValue.h"
#include <string>
#include <sstream>
#include <stdexcept>

inline std::string styleValueToString(const StyleValue& sv) {
    if (sv.type == StyleValue::NUMERIC) {
        std::ostringstream oss;
        oss << sv.numeric_val;
        std::string numStr = oss.str();
        // Only clean up trailing zeros if a decimal point exists.
        if (numStr.find('.') != std::string::npos) {
            // Remove trailing zeros.
            numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
            // If the last character is now a decimal point, remove it.
            if (!numStr.empty() && numStr.back() == '.') {
                numStr.pop_back();
            }
        }
        return numStr + sv.unit;
    }
    if (sv.type == StyleValue::BOOL) {
        throw std::runtime_error("Cannot convert a boolean expression result to a final CSS value.");
    }
    return sv.string_val;
}
