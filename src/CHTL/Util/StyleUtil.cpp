#include "StyleUtil.h"
#include <sstream>
#include <stdexcept>
#include <cctype>

std::string styleValueToString(const StyleValue& sv) {
    switch (sv.type) {
        case StyleValue::NUMERIC: {
            std::ostringstream oss;
            oss.precision(15); // Preserve precision
            oss << sv.numeric_val;
            std::string numStr = oss.str();
            // Clean up trailing zeros for non-decimal numbers
            if (numStr.find('.') != std::string::npos) {
                numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
                if (numStr.back() == '.') {
                    numStr.pop_back();
                }
            }
            return numStr + sv.unit;
        }
        case StyleValue::STRING:
            return sv.string_val;
        case StyleValue::EMPTY:
            return "";
        case StyleValue::BOOL:
            throw std::runtime_error("Cannot convert a boolean expression result to a string style value.");
    }
    return ""; // Should not be reached
}

StyleValue decoupleNumericValue(const std::string& s) {
    std::stringstream ss(s);
    std::string temp;

    // Read word by word from the string stream
    while (ss >> temp) {
        // Try to parse the current word as a number
        try {
            size_t unit_pos = temp.find_first_not_of("-.0123456789");
            double value = std::stod(temp.substr(0, unit_pos));
            std::string unit = (unit_pos != std::string::npos) ? temp.substr(unit_pos) : "";

            // A valid unit should be either '%' or purely alphabetic.
            bool unit_valid = true;
            if (!unit.empty()) {
                if (unit == "%") {
                    // '%' is a valid unit.
                } else {
                    for (char c : unit) {
                        if (!isalpha(c)) {
                            unit_valid = false;
                            break;
                        }
                    }
                }
            }

            if (unit_valid) {
                // Successfully parsed a number and a valid unit.
                return {StyleValue::NUMERIC, value, unit};
            }
        } catch (...) {
            // This word is not a number, so we continue to the next one.
            continue;
        }
    }

    // If we get through the whole string without finding a number, return EMPTY.
    return {StyleValue::EMPTY};
}
