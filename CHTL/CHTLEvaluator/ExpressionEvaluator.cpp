#include "ExpressionEvaluator.h"
#include "CHTLNode/ConditionalExpressionNode.h"
#include "CHTLNode/ReferenceNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/LiteralNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLUtil/DocumentTraverser.h"
#include <stdexcept>
#include <cmath>

namespace CHTL {

EvaluatedValue ExpressionEvaluator::evaluate(const ExpressionNode* expression, const BaseNode* root, const StyleNode* style_context) {
    if (!expression) {
        throw std::runtime_error("Cannot evaluate a null expression.");
    }

    switch (expression->getType()) {
        case ExpressionType::LITERAL: {
            const auto* literal = static_cast<const LiteralNode*>(expression);
            if (literal->getValue().type == TokenType::NUMBER) {
                return {EvaluatedValue::Type::NUMBER, std::stod(literal->getValue().value), "", literal->getUnit()};
            } else if (literal->getValue().type == TokenType::IDENTIFIER && style_context) {
                for (const auto& prop : style_context->getProperties()) {
                    if (prop->getKey() == literal->getValue().value) {
                        if (prop->getValue() == expression) {
                             throw std::runtime_error("Circular property reference detected: " + prop->getKey());
                        }
                        return evaluate(prop->getValue(), root, style_context);
                    }
                }
                 return {EvaluatedValue::Type::STRING, 0.0, literal->getValue().value, ""};
            }
            else {
                return {EvaluatedValue::Type::STRING, 0.0, literal->getValue().value, ""};
            }
        }
        case ExpressionType::BINARY_OP: {
            const auto* binaryOp = static_cast<const BinaryOpNode*>(expression);
            EvaluatedValue left = evaluate(binaryOp->getLeft(), root, style_context);
            EvaluatedValue right = evaluate(binaryOp->getRight(), root, style_context);

            if (left.type == EvaluatedValue::Type::STRING || right.type == EvaluatedValue::Type::STRING) {
                 if (binaryOp->getOperator().type != TokenType::EQUAL_EQUAL && binaryOp->getOperator().type != TokenType::BANG_EQUAL)
                    throw std::runtime_error("Arithmetic operations are not supported on string literals.");
            }

            switch (binaryOp->getOperator().type) {
                case TokenType::PLUS:
                    if (left.unit.empty()) return {EvaluatedValue::Type::NUMBER, left.number_value + right.number_value, "", right.unit};
                    if (right.unit.empty()) return {EvaluatedValue::Type::NUMBER, left.number_value + right.number_value, "", left.unit};
                    if (left.unit == right.unit) return {EvaluatedValue::Type::NUMBER, left.number_value + right.number_value, "", left.unit};
                    throw std::runtime_error("Incompatible units for addition: '" + left.unit + "' and '" + right.unit + "'");
                case TokenType::MINUS:
                    if (left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot subtract a value with unit from a unit-less value.");
                    if (right.unit.empty()) return {EvaluatedValue::Type::NUMBER, left.number_value - right.number_value, "", left.unit};
                    if (left.unit == right.unit) return {EvaluatedValue::Type::NUMBER, left.number_value - right.number_value, "", left.unit};
                    throw std::runtime_error("Incompatible units for subtraction: '" + left.unit + "' and '" + right.unit + "'");
                case TokenType::STAR:
                    if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot multiply two values with units.");
                    return {EvaluatedValue::Type::NUMBER, left.number_value * right.number_value, "", !left.unit.empty() ? left.unit : right.unit};
                case TokenType::SLASH:
                    if (left.unit == right.unit && !left.unit.empty()) return {EvaluatedValue::Type::NUMBER, left.number_value / right.number_value, "", ""};
                    if (!right.unit.empty()) throw std::runtime_error("Cannot divide by a value with a unit.");
                    return {EvaluatedValue::Type::NUMBER, left.number_value / right.number_value, "", left.unit};
                case TokenType::PERCENT:
                    if (!left.unit.empty() || !right.unit.empty()) throw std::runtime_error("Modulo operator does not support units.");
                    return {EvaluatedValue::Type::NUMBER, std::fmod(left.number_value, right.number_value), "", ""};
                case TokenType::STAR_STAR:
                    if (!left.unit.empty() || !right.unit.empty()) throw std::runtime_error("Power operator does not support units.");
                    return {EvaluatedValue::Type::NUMBER, std::pow(left.number_value, right.number_value), "", ""};
                case TokenType::GREATER:
                    return {EvaluatedValue::Type::NUMBER, left.number_value > right.number_value ? 1.0 : 0.0, "", ""};
                case TokenType::LESS:
                    return {EvaluatedValue::Type::NUMBER, left.number_value < right.number_value ? 1.0 : 0.0, "", ""};
                case TokenType::GREATER_EQUAL:
                    return {EvaluatedValue::Type::NUMBER, left.number_value >= right.number_value ? 1.0 : 0.0, "", ""};
                case TokenType::LESS_EQUAL:
                    return {EvaluatedValue::Type::NUMBER, left.number_value <= right.number_value ? 1.0 : 0.0, "", ""};
                case TokenType::EQUAL_EQUAL:
                     if (left.type != right.type) return {EvaluatedValue::Type::NUMBER, 0.0, "", ""};
                     if(left.type == EvaluatedValue::Type::STRING) return {EvaluatedValue::Type::NUMBER, left.string_value == right.string_value ? 1.0 : 0.0, "", ""};
                    return {EvaluatedValue::Type::NUMBER, left.number_value == right.number_value && left.unit == right.unit ? 1.0 : 0.0, "", ""};
                case TokenType::BANG_EQUAL:
                     if (left.type != right.type) return {EvaluatedValue::Type::NUMBER, 1.0, "", ""};
                     if(left.type == EvaluatedValue::Type::STRING) return {EvaluatedValue::Type::NUMBER, left.string_value != right.string_value ? 1.0 : 0.0, "", ""};
                    return {EvaluatedValue::Type::NUMBER, left.number_value != right.number_value || left.unit != right.unit ? 1.0 : 0.0, "", ""};
                case TokenType::AMPERSAND_AMPERSAND:
                    return {EvaluatedValue::Type::NUMBER, (left.number_value != 0.0 && right.number_value != 0.0) ? 1.0 : 0.0, "", ""};
                case TokenType::PIPE_PIPE:
                    return {EvaluatedValue::Type::NUMBER, (left.number_value != 0.0 || right.number_value != 0.0) ? 1.0 : 0.0, "", ""};
                default:
                    throw std::runtime_error("Unsupported binary operator.");
            }
        }
        case ExpressionType::CONDITIONAL: {
            const auto* condNode = static_cast<const ConditionalExpressionNode*>(expression);
            EvaluatedValue condition = evaluate(condNode->getCondition(), root, style_context);
            if (condition.type != EvaluatedValue::Type::NUMBER) {
                throw std::runtime_error("Condition must evaluate to a number.");
            }
            if (condition.number_value != 0.0) {
                return evaluate(condNode->getTrueExpression(), root, style_context);
            } else {
                return evaluate(condNode->getFalseExpression(), root, style_context);
            }
        }
        case ExpressionType::REFERENCE: {
            const auto* refNode = static_cast<const ReferenceNode*>(expression);
            if (!root) {
                throw std::runtime_error("Cannot resolve property reference without a document root.");
            }
            const ElementNode* referencedElement = DocumentTraverser::find(root, refNode->getSelector());
            if (!referencedElement) {
                throw std::runtime_error("Could not find element with selector: " + refNode->getSelector());
            }
            const StyleNode* referencedStyle = referencedElement->getStyle();
            if (!referencedStyle) {
                throw std::runtime_error("Referenced element has no style block.");
            }
            for (const auto& prop : referencedStyle->getProperties()) {
                if (prop->getKey() == refNode->getPropertyName()) {
                    return evaluate(prop->getValue(), root, referencedStyle);
                }
            }
            throw std::runtime_error("Property '" + refNode->getPropertyName() + "' not found on element with selector '" + refNode->getSelector() + "'");
        }
        default:
            throw std::runtime_error("Unsupported expression type.");
    }
}

} // namespace CHTL