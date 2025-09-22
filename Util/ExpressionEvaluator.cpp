#include "ExpressionEvaluator.h"
#include <algorithm> // for std::find_if
#include <cctype>    // for std::isalpha

ValueUnit ExpressionEvaluator::parseValueUnit(const std::string& lexeme) {
    // Find the first alphabetic character which marks the start of the unit.
    auto it = std::find_if(lexeme.begin(), lexeme.end(), ::isalpha);

    std::string numPart(lexeme.begin(), it);
    std::string unitPart(it, lexeme.end());

    try {
        double value = std::stod(numPart);
        return {value, unitPart};
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid numeric value in expression: " + lexeme);
    }
}

std::string ExpressionEvaluator::evaluate(const std::vector<Token>& tokens) {
    if (tokens.empty()) {
        return "";
    }

    // If there's only one token, it's a literal value.
    if (tokens.size() == 1) {
        if (tokens[0].type == TokenType::TOKEN_STRING) {
             return tokens[0].lexeme.substr(1, tokens[0].lexeme.length() - 2);
        }
        return tokens[0].lexeme;
    }

    // Start with the first operand.
    ValueUnit result = parseValueUnit(tokens[0].lexeme);

    // Process the rest of the expression in operator-operand pairs.
    for (size_t i = 1; i < tokens.size(); i += 2) {
        const Token& op = tokens[i];
        if (i + 1 >= tokens.size()) {
            throw std::runtime_error("Incomplete expression: operator without operand.");
        }
        const Token& operandToken = tokens[i+1];
        ValueUnit operand = parseValueUnit(operandToken.lexeme);

        // Per spec, units must match for arithmetic.
        if (result.unit != operand.unit) {
            throw std::runtime_error("Mismatched units in expression: '" + result.unit + "' and '" + operand.unit + "'.");
        }

        if (op.type == TokenType::TOKEN_PLUS) {
            result.value += operand.value;
        } else if (op.type == TokenType::TOKEN_MINUS) {
            result.value -= operand.value;
        } else {
            throw std::runtime_error("Unsupported operator in style expression: " + op.lexeme);
        }
    }

    return result.toString();
}
