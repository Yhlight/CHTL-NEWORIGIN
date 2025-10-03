#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <iostream>

// Helper struct for arithmetic evaluation
struct ValueWithUnit {
    double value;
    std::string unit;
};

// Helper function to parse a string like "100px" into a value and a unit
static ValueWithUnit parseValueWithUnit(const std::string& s) {
    if (s.empty()) return {0.0, ""};
    size_t i = 0;
    if (s[0] == '-' || s[0] == '+') {
        i++;
    }
    while (i < s.length() && (isdigit(s[i]) || s[i] == '.')) {
        i++;
    }
    std::string num_part = s.substr(0, i);
    std::string unit_part = s.substr(i);
    if (num_part == "+" || num_part == "-") {
        return {std::stod(num_part + "0"), unit_part};
    }
    return {num_part.empty() ? 0.0 : std::stod(num_part), unit_part};
}


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
                    while (!operator_stack.empty() && isOperator(operator_stack.top().type) && (
                           (!isRightAssociative(token.type) && getPrecedence(operator_stack.top().type) >= getPrecedence(token.type)) ||
                           (isRightAssociative(token.type) && getPrecedence(operator_stack.top().type) > getPrecedence(token.type))
                    )) {
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
        case TokenType::Slash:
        case TokenType::Percent: return 6;
        case TokenType::Power: return 7;
        default: return 0;
    }
}

bool ExpressionEvaluator::isOperator(TokenType type) {
    return getPrecedence(type) > 0;
}

bool ExpressionEvaluator::isRightAssociative(TokenType type) {
    return type == TokenType::Power;
}

std::string ExpressionEvaluator::resolveConditionalProperty(const ConditionalPropertyValue& propValue) {
    for (const auto& branch : propValue.branches) {
        if (evaluate(branch.condition)) {
            return branch.trueValue;
        }
    }
    return propValue.elseValue; // Return the else value, or an empty string if it doesn't exist.
}

static ValueWithUnit evaluateArithmeticRPN(const std::vector<Token>& rpnTokens) {
    std::stack<ValueWithUnit> value_stack;

    auto is_op = [](TokenType type){
        switch (type) {
            case TokenType::Plus: case TokenType::Minus: case TokenType::Asterisk:
            case TokenType::Slash: case TokenType::Percent: case TokenType::Power:
                return true;
            default:
                return false;
        }
    };

    for (const auto& token : rpnTokens) {
        if (token.type == TokenType::Identifier || token.type == TokenType::Number) {
            value_stack.push(parseValueWithUnit(token.value));
        } else if (is_op(token.type)) {
            if (value_stack.size() < 2) throw std::runtime_error("Invalid arithmetic expression: not enough operands.");
            ValueWithUnit right = value_stack.top(); value_stack.pop();
            ValueWithUnit left = value_stack.top(); value_stack.pop();
            ValueWithUnit result;

            switch(token.type) {
                case TokenType::Plus:
                case TokenType::Minus:
                    if (left.unit != right.unit && !left.unit.empty() && !right.unit.empty()) {
                        throw std::runtime_error("Unit mismatch for '+' or '-' operation.");
                    }
                    result.value = (token.type == TokenType::Plus) ? (left.value + right.value) : (left.value - right.value);
                    result.unit = !left.unit.empty() ? left.unit : right.unit;
                    break;
                case TokenType::Asterisk:
                    result.value = left.value * right.value;
                    if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot multiply two values with units.");
                    result.unit = !left.unit.empty() ? left.unit : right.unit;
                    break;
                case TokenType::Slash:
                    if (right.value == 0) throw std::runtime_error("Division by zero.");
                    result.value = left.value / right.value;
                     if (!left.unit.empty() && !right.unit.empty()) {
                         if (left.unit != right.unit) throw std::runtime_error("Unit mismatch for division.");
                         result.unit = ""; // Units cancel out
                    } else {
                        result.unit = left.unit;
                    }
                    break;
                 case TokenType::Percent:
                    if (static_cast<long long>(right.value) == 0) throw std::runtime_error("Modulo by zero.");
                    result.value = fmod(left.value, right.value);
                    result.unit = left.unit; // Unit of left operand is preserved
                    break;
                case TokenType::Power:
                    if (!right.unit.empty()) throw std::runtime_error("Exponent cannot have a unit.");
                    result.value = pow(left.value, right.value);
                    result.unit = left.unit; // Unit of left operand is preserved
                    break;
                default:
                     throw std::runtime_error("Unsupported operator in arithmetic expression.");
            }
            value_stack.push(result);
        }
    }
    if (value_stack.size() != 1) throw std::runtime_error("Invalid arithmetic expression: stack size is not 1 after evaluation.");
    return value_stack.top();
}

std::string ExpressionEvaluator::resolveArithmeticExpression(const ArithmeticExpression& expression) {
    if (expression.tokens.empty()) return "";

    try {
        auto rpn = shuntingYard(expression.tokens);
        ValueWithUnit result = evaluateArithmeticRPN(rpn);

        std::stringstream ss;
        if (result.value == static_cast<long long>(result.value)) {
             ss << static_cast<long long>(result.value);
        } else {
             ss << result.value;
        }
        ss << result.unit;
        return ss.str();
    } catch (const std::exception& e) {
        std::cerr << "Error evaluating arithmetic expression: " << e.what() << std::endl;
        return "";
    }
}