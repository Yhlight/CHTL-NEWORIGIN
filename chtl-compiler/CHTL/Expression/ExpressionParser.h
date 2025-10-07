#pragma once

#include "../CHTLLexer/Token.h"
#include "ExpressionEvaluator.h"
#include <string>
#include <vector>
#include <stdexcept>

class ExpressionParser {
public:
    // This is a simplified parser for binary expressions like "10px + 20px".
    // It does not yet handle operator precedence.
    static std::string parseAndEvaluate(const std::vector<Token>& tokens) {
        if (tokens.empty()) {
            return "";
        }

        // If it's just a single value, return it.
        if (tokens.size() == 1) {
            return tokens[0].value;
        }

        // Handle simple binary expression: operand operator operand
        if (tokens.size() == 3) {
            const auto& left = tokens[0];
            const auto& op = tokens[1];
            const auto& right = tokens[2];

            if ((left.type == TokenType::Identifier || left.type == TokenType::UnquotedLiteral) &&
                (right.type == TokenType::Identifier || right.type == TokenType::UnquotedLiteral)) {

                std::string op_str;
                if (op.type == TokenType::Plus) op_str = "+";
                else if (op.type == TokenType::Minus) op_str = "-";
                else if (op.type == TokenType::Star) op_str = "*";
                else if (op.type == TokenType::Slash) op_str = "/";
                else {
                    throw std::runtime_error("Unsupported operator in expression.");
                }

                return ExpressionEvaluator::evaluate(left.value, op_str, right.value);
            }
        }

        // For more complex expressions, or if not a simple binary operation,
        // just concatenate the values for now.
        std::string result;
        for (const auto& token : tokens) {
            result += token.value;
        }
        return result;
    }
};