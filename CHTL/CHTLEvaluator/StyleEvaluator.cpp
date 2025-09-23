#include "StyleEvaluator.h"
#include "../CHTLExpr/BinaryOpNode.h"
#include "../CHTLExpr/TernaryOpNode.h"
#include "../CHTLExpr/PropertyAccessNode.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

// A variant-like struct to hold the result of an evaluation
struct EvaluatedValue {
    bool isNumeric;
    ParsedValue numericVal;
    std::string stringVal;
};

// Helper to convert the numeric part of an EvaluatedValue to a string
std::string valueToString(const EvaluatedValue& ev) {
    if (!ev.isNumeric) return ev.stringVal;
    const ParsedValue& pv = ev.numericVal;
    if (pv.value == floor(pv.value)) return std::to_string(static_cast<int>(pv.value)) + pv.unit;
    return std::to_string(pv.value) + pv.unit;
}

// Helper to check if an expression tree contains a property access
bool hasPropertyAccess(const std::shared_ptr<BaseExprNode>& expr) {
    if (!expr) return false;
    if (expr->getType() == ExprNodeType::PropertyAccess) return true;
    if (expr->getType() == ExprNodeType::BinaryOp) {
        auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr);
        return hasPropertyAccess(binOp->left) || hasPropertyAccess(binOp->right);
    }
    if (expr->getType() == ExprNodeType::TernaryOp) {
        auto terOp = std::dynamic_pointer_cast<TernaryOpNode>(expr);
        return hasPropertyAccess(terOp->condition) || hasPropertyAccess(terOp->trueExpr) || hasPropertyAccess(terOp->falseExpr);
    }
    return false;
}


void StyleEvaluator::evaluate(const std::shared_ptr<ElementNode>& root) {
    if (!root) return;
    evaluateNode(root);
}

void StyleEvaluator::evaluateNode(const std::shared_ptr<ElementNode>& node) {
    std::map<std::string, EvaluatedValue> localContext;
    std::map<std::string, std::shared_ptr<BaseExprNode>> dependentStyles;

    // Pass 1: Evaluate simple properties
    for (const auto& stylePair : node->inlineStyles) {
        if (hasPropertyAccess(stylePair.second)) {
            dependentStyles[stylePair.first] = stylePair.second;
        } else {
            try {
                localContext[stylePair.first] = evaluateExpression(stylePair.second, localContext);
                node->finalStyles[stylePair.first] = valueToString(localContext[stylePair.first]);
            } catch (const std::exception& e) {
                node->finalStyles[stylePair.first] = "ERROR: " + std::string(e.what());
            }
        }
    }

    // Pass 2: Evaluate dependent properties
    for (const auto& stylePair : dependentStyles) {
        try {
            localContext[stylePair.first] = evaluateExpression(stylePair.second, localContext);
            node->finalStyles[stylePair.first] = valueToString(localContext[stylePair.first]);
        } catch (const std::exception& e) {
            node->finalStyles[stylePair.first] = "ERROR: " + std::string(e.what());
        }
    }

    // Recursively evaluate for all children
    for (const auto& child : node->children) {
        if (child->getType() == NodeType::Element) {
            evaluateNode(std::dynamic_pointer_cast<ElementNode>(child));
        }
    }
}

EvaluatedValue StyleEvaluator::evaluateExpression(const std::shared_ptr<BaseExprNode>& expr, const std::map<std::string, EvaluatedValue>& context) {
    if (!expr) throw std::runtime_error("Cannot evaluate null expression.");

    switch (expr->getType()) {
        case ExprNodeType::Literal: {
            auto literal = std::dynamic_pointer_cast<LiteralNode>(expr);
            if (literal->isNumeric) return {true, literal->numericValue, ""};
            return {false, {}, literal->rawValue};
        }
        case ExprNodeType::PropertyAccess: {
            auto access = std::dynamic_pointer_cast<PropertyAccessNode>(expr);
            if (context.count(access->propertyName)) {
                // It's a valid reference to another property
                return context.at(access->propertyName);
            } else {
                // It's not a reference, so treat it as a string literal
                return {false, {}, access->propertyName};
            }
        }
        case ExprNodeType::TernaryOp: {
            auto ternOp = std::dynamic_pointer_cast<TernaryOpNode>(expr);
            EvaluatedValue cond = evaluateExpression(ternOp->condition, context);
            if (!cond.isNumeric) throw std::runtime_error("Condition for ternary must be numeric.");
            return (cond.numericVal.value != 0) ? evaluateExpression(ternOp->trueExpr, context) : evaluateExpression(ternOp->falseExpr, context);
        }
        case ExprNodeType::BinaryOp: {
            auto binOp = std::dynamic_pointer_cast<BinaryOpNode>(expr);
            EvaluatedValue left = evaluateExpression(binOp->left, context);
            EvaluatedValue right = evaluateExpression(binOp->right, context);

            if (!left.isNumeric || !right.isNumeric) {
                 if (binOp->op == TokenType::EQUAL_EQUAL) return {true, { (double)(left.stringVal == right.stringVal), ""}, ""};
                 if (binOp->op == TokenType::NOT_EQUAL) return {true, { (double)(left.stringVal != right.stringVal), ""}, ""};
                 throw std::runtime_error("Invalid operands for binary operation.");
            }

            const ParsedValue& leftPv = left.numericVal;
            const ParsedValue& rightPv = right.numericVal;

            if (!leftPv.unit.empty() && !rightPv.unit.empty() && leftPv.unit != rightPv.unit) {
                throw std::runtime_error("Incompatible units: " + leftPv.unit + " and " + rightPv.unit);
            }
            std::string resultUnit = !leftPv.unit.empty() ? leftPv.unit : rightPv.unit;
            double resultValue = 0;

            switch (binOp->op) {
                case TokenType::PLUS: resultValue = leftPv.value + rightPv.value; break;
                case TokenType::MINUS: resultValue = leftPv.value - rightPv.value; break;
                case TokenType::STAR: resultValue = leftPv.value * rightPv.value; break;
                case TokenType::SLASH:
                    if (rightPv.value == 0) throw std::runtime_error("Division by zero.");
                    resultValue = leftPv.value / rightPv.value; break;
                case TokenType::PERCENT: resultValue = fmod(leftPv.value, rightPv.value); break;
                case TokenType::DOUBLE_STAR: resultValue = pow(leftPv.value, rightPv.value); break;
                case TokenType::GREATER: resultValue = leftPv.value > rightPv.value; break;
                case TokenType::LESS: resultValue = leftPv.value < rightPv.value; break;
                case TokenType::EQUAL_EQUAL: resultValue = leftPv.value == rightPv.value; break;
                case TokenType::NOT_EQUAL: resultValue = leftPv.value != rightPv.value; break;
                case TokenType::LOGICAL_AND: resultValue = leftPv.value && rightPv.value; break;
                case TokenType::LOGICAL_OR: resultValue = leftPv.value || rightPv.value; break;
                default: throw std::runtime_error("Unsupported binary operator.");
            }
            return {true, {resultValue, resultUnit}, ""};
        }
        default:
            throw std::runtime_error("Unsupported expression node type for evaluation.");
    }
}
