#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <charconv>
#include <cctype>
#include <algorithm>

namespace CHTL {

ExpressionEvaluator::ExpressionEvaluator(const std::shared_ptr<ElementNode>& context_element)
    : context_element(context_element) {}

std::any ExpressionEvaluator::evaluate(const std::shared_ptr<ExpressionNode>& expr) {
    return visit(expr);
}

std::any ExpressionEvaluator::visit(const std::shared_ptr<ExpressionNode>& expr) {
    switch (expr->getType()) {
        case NodeType::NODE_EXPRESSION_LITERAL:
            return visitLiteral(std::dynamic_pointer_cast<LiteralExpressionNode>(expr));
        case NodeType::NODE_EXPRESSION_UNARY:
            return visitUnary(std::dynamic_pointer_cast<UnaryExpressionNode>(expr));
        case NodeType::NODE_EXPRESSION_BINARY:
            return visitBinary(std::dynamic_pointer_cast<BinaryExpressionNode>(expr));
        case NodeType::NODE_EXPRESSION_GROUPING:
            return visitGrouping(std::dynamic_pointer_cast<GroupingExpressionNode>(expr));
        case NodeType::NODE_EXPRESSION_VARIABLE:
            return visitVariable(std::dynamic_pointer_cast<VariableExpressionNode>(expr));
        default:
            throw std::runtime_error("Unknown expression node type.");
    }
}

std::any ExpressionEvaluator::visitLiteral(const std::shared_ptr<LiteralExpressionNode>& expr) {
    double value;
    auto result = std::from_chars(expr->value.data(), expr->value.data() + expr->value.size(), value);
    if (result.ec == std::errc()) {
        return value;
    }
    return expr->value; // It's a string literal
}

std::any ExpressionEvaluator::visitUnary(const std::shared_ptr<UnaryExpressionNode>& expr) {
    auto right = evaluate(expr->right);
    if (expr->op.type == TokenType::TOKEN_MINUS) {
        return -toDouble(right);
    }
    return right;
}

std::any ExpressionEvaluator::visitBinary(const std::shared_ptr<BinaryExpressionNode>& expr) {
    auto left = evaluate(expr->left);
    auto right = evaluate(expr->right);

    switch (expr->op.type) {
        case TokenType::TOKEN_PLUS:
            return toDouble(left) + toDouble(right);
        case TokenType::TOKEN_MINUS:
            return toDouble(left) - toDouble(right);
        case TokenType::TOKEN_MULTIPLY:
            return toDouble(left) * toDouble(right);
        case TokenType::TOKEN_DIVIDE:
            return toDouble(left) / toDouble(right);
        case TokenType::TOKEN_GT:
            return toDouble(left) > toDouble(right);
        case TokenType::TOKEN_GTE:
            return toDouble(left) >= toDouble(right);
        case TokenType::TOKEN_LT:
            return toDouble(left) < toDouble(right);
        case TokenType::TOKEN_LTE:
            return toDouble(left) <= toDouble(right);
        case TokenType::TOKEN_EQ:
            return toDouble(left) == toDouble(right);
        case TokenType::TOKEN_NEQ:
            return toDouble(left) != toDouble(right);
        case TokenType::TOKEN_AND:
            return isTruthy(left) && isTruthy(right);
        case TokenType::TOKEN_OR:
            return isTruthy(left) || isTruthy(right);
        default:
            throw std::runtime_error("Unsupported binary operator.");
    }
}

std::any ExpressionEvaluator::visitGrouping(const std::shared_ptr<GroupingExpressionNode>& expr) {
    return evaluate(expr->expression);
}

std::any ExpressionEvaluator::visitVariable(const std::shared_ptr<VariableExpressionNode>& expr) {
    if (context_element && context_element->hasAttribute(expr->name)) {
        std::string attr_val_str = context_element->getAttribute(expr->name);
        attr_val_str.erase(std::remove_if(attr_val_str.begin(), attr_val_str.end(),
                                       [](char c) { return !std::isdigit(c) && c != '.'; }),
                         attr_val_str.end());
        double value;
        auto result = std::from_chars(attr_val_str.data(), attr_val_str.data() + attr_val_str.size(), value);
        if (result.ec == std::errc()) {
            return value;
        }
    }
    throw std::runtime_error("Undefined variable: " + expr->name);
}

bool ExpressionEvaluator::isTruthy(const std::any& value) {
    if (value.type() == typeid(double)) {
        return std::any_cast<double>(value) != 0.0;
    }
    if (value.type() == typeid(std::string)) {
        return !std::any_cast<std::string>(value).empty();
    }
    if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value);
    }
    return false;
}

double ExpressionEvaluator::toDouble(const std::any& value) {
    if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    }
    throw std::runtime_error("Operand must be a number.");
}

}