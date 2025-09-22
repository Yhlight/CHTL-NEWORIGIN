#pragma once

#include "CHTL/CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <stdexcept>

// A simple struct to hold a numeric value and its unit.
struct ValueUnit {
    double value;
    std::string unit;

    // Helper to convert back to string
    std::string toString() const {
        // Avoid trailing zeros for clean output
        std::string valStr = std::to_string(value);
        valStr.erase(valStr.find_last_not_of('0') + 1, std::string::npos);
        if (valStr.back() == '.') {
            valStr.pop_back();
        }
        return valStr + unit;
    }
};

class ExpressionEvaluator {
public:
    // Evaluates a sequence of tokens representing a style expression.
    static std::string evaluate(const std::vector<Token>& tokens);

private:
    // Parses a string like "100px" into a ValueUnit struct.
    static ValueUnit parseValueUnit(const std::string& lexeme);
};
