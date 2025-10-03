#include "ExpressionEvaluator.h"
#include "../../CHTL/CHTLNode/ExpressionNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include "../../CHTL/CHTLNode/BinaryOpNode.h"
#include <stdexcept>
#include <string>
#include <sstream>

namespace {
    struct NumericValue {
        double value;
        std::string unit;
    };

    NumericValue parse_value(const std::string& s) {
        if (s.empty()) return {0.0, ""};

        size_t first_alpha = std::string::npos;
        for (size_t i = 0; i < s.length(); ++i) {
            if (isalpha(s[i]) || s[i] == '%') {
                first_alpha = i;
                break;
            }
        }

        if (first_alpha == std::string::npos) {
            return {std::stod(s), ""};
        }

        double val = std::stod(s.substr(0, first_alpha));
        std::string unit = s.substr(first_alpha);
        return {val, unit};
    }

    std::string format_value(const NumericValue& nv) {
        std::stringstream ss;
        ss << nv.value << nv.unit;
        return ss.str();
    }

    std::string evaluate_recursive(const ExpressionNode* node) {
        if (!node) {
            throw std::runtime_error("Cannot evaluate null expression node.");
        }

        switch (node->getType()) {
            case NodeType::Value: {
                const auto* valueNode = static_cast<const ValueNode*>(node);
                return valueNode->getValue();
            }
            case NodeType::BinaryOp: {
                const auto* opNode = static_cast<const BinaryOpNode*>(node);

                NumericValue left = parse_value(evaluate_recursive(opNode->getLeft()));
                NumericValue right = parse_value(evaluate_recursive(opNode->getRight()));
                NumericValue result;

                switch (opNode->getOp()) {
                    case TokenType::PLUS:
                        if (left.unit != right.unit) throw std::runtime_error("Mismatched units for addition.");
                        result = {left.value + right.value, left.unit};
                        break;
                    case TokenType::MINUS:
                        if (left.unit != right.unit) throw std::runtime_error("Mismatched units for subtraction.");
                        result = {left.value - right.value, left.unit};
                        break;
                    case TokenType::STAR:
                        if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot multiply two values with units.");
                        result = {left.value * right.value, left.unit.empty() ? right.unit : left.unit};
                        break;
                    case TokenType::SLASH:
                        if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot divide two values with units.");
                        if (right.value == 0) throw std::runtime_error("Division by zero.");
                        result = {left.value / right.value, left.unit.empty() ? right.unit : left.unit};
                        break;
                    default:
                        throw std::runtime_error("Unsupported binary operator.");
                }
                return format_value(result);
            }
            default:
                throw std::runtime_error("Unknown or unsupported expression node type for evaluation.");
        }
    }
}

std::string ExpressionEvaluator::evaluate(const ExpressionNode* node) {
    return evaluate_recursive(node);
}