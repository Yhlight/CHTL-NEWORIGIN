#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <cctype>
#include <iostream>

ExpressionEvaluator::ExpressionEvaluator(const ElementNode& context) : context(context) {}

bool ExpressionEvaluator::evaluate(const std::vector<Token>& tokens) {
    if (tokens.empty()) return false;
    if (tokens.size() == 1 && tokens[0].value == "true") return true;
    if (tokens.size() == 1 && tokens[0].value == "false") return false;

    try {
        auto rpn = shuntingYard(tokens);
        return evaluateRPN(rpn);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<Token> ExpressionEvaluator::shuntingYard(const std::vector<Token>& tokens) {
    std::vector<Token> output_queue;
    std::stack<Token> operator_stack;

    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::Number:
            case TokenType::Identifier:
                output_queue.push_back(token);
                break;
            case TokenType::LParen:
                operator_stack.push(token);
                break;
            case TokenType::RParen:
                while (!operator_stack.empty() && operator_stack.top().type != TokenType::LParen) {
                    output_queue.push_back(operator_stack.top());
                    operator_stack.pop();
                }
                if (!operator_stack.empty()) operator_stack.pop(); // Pop '('
                break;
            default: // Operator
                if (isOperator(token.type)) {
                    while (!operator_stack.empty() && isOperator(operator_stack.top().type) &&
                           getPrecedence(operator_stack.top().type) >= getPrecedence(token.type)) {
                        output_queue.push_back(operator_stack.top());
                        operator_stack.pop();
                    }
                    operator_stack.push(token);
                }
                break;
        }
    }

    while (!operator_stack.empty()) {
        output_queue.push_back(operator_stack.top());
        operator_stack.pop();
    }

    return output_queue;
}

double ExpressionEvaluator::evaluateRPN(const std::vector<Token>& rpnTokens) {
    std::stack<double> value_stack;

    for (const auto& token : rpnTokens) {
        if (token.type == TokenType::Number) {
            value_stack.push(std::stod(token.value));
        } else if (token.type == TokenType::Identifier) {
            value_stack.push(getValue(token.value));
        } else if (isOperator(token.type)) {
            if (value_stack.size() < 2) throw std::runtime_error("Invalid expression");
            double right = value_stack.top(); value_stack.pop();
            double left = value_stack.top(); value_stack.pop();

            double result = 0.0;
            switch(token.type) {
                case TokenType::Plus: result = left + right; break;
                case TokenType::Minus: result = left - right; break;
                case TokenType::Asterisk: result = left * right; break;
                case TokenType::Slash: result = left / right; break;
                case TokenType::GreaterThan: result = left > right; break;
                case TokenType::LessThan: result = left < right; break;
                case TokenType::EqualTo: result = left == right; break;
                case TokenType::NotEqualTo: result = left != right; break;
                case TokenType::GreaterThanOrEqual: result = left >= right; break;
                case TokenType::LessThanOrEqual: result = left <= right; break;
                case TokenType::LogicalAnd: result = left && right; break;
                case TokenType::LogicalOr: result = left || right; break;
                default: throw std::runtime_error("Unknown operator");
            }
            value_stack.push(result);
        }
    }

    if (value_stack.size() != 1) throw std::runtime_error("Invalid expression");
    return value_stack.top();
}

double ExpressionEvaluator::getValue(const std::string& identifier) {
    // First, check the element's own attributes.
    const auto& attributes = context.getAttributes();
    auto attr_it = attributes.find(identifier);
    if (attr_it != attributes.end()) {
        std::string numericPart;
        for (char c : attr_it->second) {
            if (isdigit(c) || c == '.') {
                numericPart += c;
            } else if (!numericPart.empty()) {
                break;
            }
        }
        if (!numericPart.empty()) {
            return std::stod(numericPart);
        }
    }

    // Then, check properties within any child StyleNodes.
    for (const auto& child : context.getChildren()) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->getProperties()) {
                if (prop.first == identifier) {
                    // Ensure we only try to get a value from non-conditional, simple string properties.
                    if (const std::string* value_ptr = std::get_if<std::string>(&prop.second)) {
                        std::string numericPart;
                        for (char c : *value_ptr) {
                            if (isdigit(c) || c == '.') {
                                numericPart += c;
                            } else if (!numericPart.empty()) {
                                break;
                            }
                        }
                        if (!numericPart.empty()) {
                            return std::stod(numericPart);
                        }
                    }
                }
            }
        }
    }

    // Fallback: try to interpret the identifier itself as a numeric value.
    std::string numericPart;
    for (char c : identifier) {
        if (isdigit(c) || c == '.') {
            numericPart += c;
        } else if (!numericPart.empty()) {
            break; // Stop at first non-numeric character
        }
    }
    if (!numericPart.empty()) {
        try {
            return std::stod(numericPart);
        } catch (const std::exception& e) {
            // Not a number, fall through to return 0.0
        }
    }

    return 0.0; // Identifier not found or has no numeric value
}

int ExpressionEvaluator::getPrecedence(TokenType type) {
    switch(type) {
        case TokenType::LogicalOr: return 1;
        case TokenType::LogicalAnd: return 2;
        case TokenType::EqualTo:
        case TokenType::NotEqualTo: return 3;
        case TokenType::GreaterThan:
        case TokenType::LessThan:
        case TokenType::GreaterThanOrEqual:
        case TokenType::LessThanOrEqual: return 4;
        case TokenType::Plus:
        case TokenType::Minus: return 5;
        case TokenType::Asterisk:
        case TokenType::Slash: return 6;
        default: return 0;
    }
}

bool ExpressionEvaluator::isOperator(TokenType type) {
    return getPrecedence(type) > 0;
}

std::string ExpressionEvaluator::resolveConditionalProperty(const ConditionalPropertyValue& propValue) {
    for (const auto& branch : propValue.branches) {
        if (evaluate(branch.condition)) {
            return branch.trueValue;
        }
    }
    return propValue.elseValue; // Return the else value, or an empty string if it doesn't exist.
}