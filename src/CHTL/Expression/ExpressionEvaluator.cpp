#include "ExpressionEvaluator.h"
#include <stdexcept>

namespace CHTL {

EvaluationResult ExpressionEvaluator::evaluate(const IExpressionNode& node) {
    if (const auto* num_node = dynamic_cast<const NumberNode*>(&node)) {
        return visit(*num_node);
    }
    if (const auto* bin_op_node = dynamic_cast<const BinaryOpNode*>(&node)) {
        return visit(*bin_op_node);
    }
    throw std::runtime_error("Unknown expression node type");
}

EvaluationResult ExpressionEvaluator::visit(const NumberNode& node) {
    return {node.value, node.unit};
}

EvaluationResult ExpressionEvaluator::visit(const BinaryOpNode& node) {
    EvaluationResult left_res = evaluate(*node.left);
    EvaluationResult right_res = evaluate(*node.right);

    // Basic unit compatibility check
    if (!left_res.unit.empty() && !right_res.unit.empty() && left_res.unit != right_res.unit) {
        throw std::runtime_error("Unit mismatch in expression: " + left_res.unit + " and " + right_res.unit);
    }

    std::string result_unit = !left_res.unit.empty() ? left_res.unit : right_res.unit;
    double result_value = 0.0;

    switch (node.op) {
        case TokenType::PLUS:
            result_value = left_res.value + right_res.value;
            break;
        case TokenType::MINUS:
            result_value = left_res.value - right_res.value;
            break;
        case TokenType::STAR:
            result_value = left_res.value * right_res.value;
            break;
        case TokenType::SLASH:
            if (right_res.value == 0) throw std::runtime_error("Division by zero");
            result_value = left_res.value / right_res.value;
            break;
        default:
            throw std::runtime_error("Unsupported operator in expression");
    }

    return {result_value, result_unit};
}

} // namespace CHTL
