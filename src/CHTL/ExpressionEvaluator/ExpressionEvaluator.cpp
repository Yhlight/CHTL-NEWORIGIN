#include "ExpressionEvaluator.h"
#include "../CHTLParser/Parser.h"
#include "../ExpressionNode/LiteralNode.h"
#include "../ExpressionNode/PropertyRefNode.h"
#include "../ExpressionNode/BinaryOpNode.h"
#include "../ExpressionNode/ConditionalNode.h"
#include <stdexcept>
#include <iostream>

// A simple utility to parse a string like "100px" into a number and a unit.
// This is a simplified version; a real implementation would be more robust.
std::pair<double, std::string> parseValue(const std::string& s) {
    size_t first_char = s.find_first_not_of("-.0123456789");
    if (first_char == std::string::npos) {
        return {std::stod(s), ""};
    }
    double val = std::stod(s.substr(0, first_char));
    std::string unit = s.substr(first_char);
    return {val, unit};
}

ExpressionEvaluator::ExpressionEvaluator(Parser& context) : parserContext(context) {}

std::string ExpressionEvaluator::evaluate(ExpressionBaseNode* node) {
    return visit(node);
}

std::string ExpressionEvaluator::visit(ExpressionBaseNode* node) {
    switch (node->getType()) {
        case ExpressionNodeType::Literal:
            return visitLiteralNode(static_cast<LiteralNode*>(node));
        case ExpressionNodeType::PropertyRef:
            return visitPropertyRefNode(static_cast<PropertyRefNode*>(node));
        case ExpressionNodeType::BinaryOp:
            return visitBinaryOpNode(static_cast<BinaryOpNode*>(node));
        case ExpressionNodeType::Conditional:
            return visitConditionalNode(static_cast<ConditionalNode*>(node));
        default:
            throw std::runtime_error("Unknown expression node type.");
    }
}

std::string ExpressionEvaluator::visitLiteralNode(LiteralNode* node) {
    return node->value;
}

std::string ExpressionEvaluator::visitPropertyRefNode(PropertyRefNode* node) {
    // Placeholder: Full implementation requires traversing the main CHTL AST
    // to find the specified element and its property value.
    std::cerr << "Warning: Property reference evaluation is not yet implemented." << std::endl;
    return "0px"; // Return a default value for now
}

std::string ExpressionEvaluator::visitBinaryOpNode(BinaryOpNode* node) {
    std::string leftStr = visit(node->left.get());
    std::string rightStr = visit(node->right.get());

    // For logical operators, we can treat non-zero results as true.
    if (node->op == "&&") {
        return (std::stod(leftStr) != 0 && std::stod(rightStr) != 0) ? "1" : "0";
    }
    if (node->op == "||") {
        return (std::stod(leftStr) != 0 || std::stod(rightStr) != 0) ? "1" : "0";
    }

    auto leftVal = parseValue(leftStr);
    auto rightVal = parseValue(rightStr);

    // Basic comparison operators
    if (node->op == "==") return (leftVal.first == rightVal.first) ? "1" : "0";
    if (node->op == "!=") return (leftVal.first != rightVal.first) ? "1" : "0";
    if (node->op == ">") return (leftVal.first > rightVal.first) ? "1" : "0";
    if (node->op == "<") return (leftVal.first < rightVal.first) ? "1" : "0";
    if (node->op == ">=") return (leftVal.first >= rightVal.first) ? "1" : "0";
    if (node->op == "<=") return (leftVal.first <= rightVal.first) ? "1" : "0";

    // Basic arithmetic operators.
    // This simplified version assumes units are compatible.
    double result = 0;
    if (node->op == "+") result = leftVal.first + rightVal.first;
    else if (node->op == "-") result = leftVal.first - rightVal.first;
    else if (node->op == "*") result = leftVal.first * rightVal.first;
    else if (node->op == "/") result = leftVal.first / rightVal.first;
    else {
        throw std::runtime_error("Unsupported binary operator: " + node->op);
    }

    return std::to_string(result) + leftVal.second; // Use left operand's unit
}

std::string ExpressionEvaluator::visitConditionalNode(ConditionalNode* node) {
    std::string condResult = visit(node->condition.get());
    // In C++, any non-zero value is true.
    if (std::stod(condResult) != 0) {
        return visit(node->trueExpression.get());
    } else {
        return visit(node->falseExpression.get());
    }
}