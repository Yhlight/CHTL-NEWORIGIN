#include "StyleUtil.h"
#include <sstream>
#include <stdexcept>

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
