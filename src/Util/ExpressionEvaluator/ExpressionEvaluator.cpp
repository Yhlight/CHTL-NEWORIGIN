#include "ExpressionEvaluator.h"
#include "../../CHTL/CHTLNode/ExpressionNode.h"
#include "../../CHTL/CHTLNode/ValueNode.h"
#include "../../CHTL/CHTLNode/BinaryOpNode.h"
#include "../../CHTL/CHTLNode/ReferenceNode.h"
#include "../../CHTL/CHTLNode/ConditionalNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/StylePropertyNode.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

namespace {
    // Forward declarations
    std::string evaluate_recursive(const ExpressionNode* node, const BaseNode* astRoot, const BaseNode* contextNode, std::vector<const ExpressionNode*>& call_stack);
    const ElementNode* find_node_by_selector(const std::string& selector, const BaseNode* astRoot);
    const ExpressionNode* find_property_in_node(const ElementNode* element, const std::string& propertyName);

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
            try { return {std::stod(s), ""}; } catch (const std::invalid_argument&) { return {0.0, s}; }
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

    const ElementNode* find_node_by_selector(const std::string& selector, const BaseNode* astRoot) {
        if (!astRoot || selector.empty()) return nullptr;
        std::vector<const BaseNode*> nodes_to_visit;
        nodes_to_visit.push_back(astRoot);
        while (!nodes_to_visit.empty()) {
            const BaseNode* current = nodes_to_visit.back();
            nodes_to_visit.pop_back();
            if (current->getType() == NodeType::Element) {
                const auto* elementNode = static_cast<const ElementNode*>(current);
                if (selector[0] == '.') {
                    auto attrs = elementNode->getAttributes();
                    if (attrs.count("class") && attrs.at("class") == selector.substr(1)) return elementNode;
                } else if (selector[0] == '#') {
                    auto attrs = elementNode->getAttributes();
                    if (attrs.count("id") && attrs.at("id") == selector.substr(1)) return elementNode;
                } else if (elementNode->getTagName() == selector) {
                    return elementNode;
                }
            }
            for (const auto& child : current->getChildren()) {
                nodes_to_visit.push_back(child.get());
            }
        }
        return nullptr;
    }

    const ExpressionNode* find_property_in_node(const ElementNode* element, const std::string& propertyName) {
        if (!element) return nullptr;
        for (const auto& child : element->getChildren()) {
            if (child->getType() == NodeType::Style) {
                for (const auto& styleChild : child->getChildren()) {
                    if (styleChild->getType() == NodeType::StyleProperty) {
                        const auto* propNode = static_cast<const StylePropertyNode*>(styleChild.get());
                        if (propNode->getName() == propertyName) return propNode->getValue();
                    }
                }
            }
        }
        return nullptr;
    }

    std::string evaluate_recursive(const ExpressionNode* node, const BaseNode* astRoot, const BaseNode* contextNode, std::vector<const ExpressionNode*>& call_stack) {
        if (!node) throw std::runtime_error("Cannot evaluate null expression node.");
        if (std::find(call_stack.begin(), call_stack.end(), node) != call_stack.end()) throw std::runtime_error("Circular property reference detected.");
        call_stack.push_back(node);

        std::string result_str;
        switch (node->getType()) {
            case NodeType::Value: {
                result_str = static_cast<const ValueNode*>(node)->getValue();
                break;
            }
            case NodeType::BinaryOp: {
                const auto* opNode = static_cast<const BinaryOpNode*>(node);
                NumericValue left = parse_value(evaluate_recursive(opNode->getLeft(), astRoot, contextNode, call_stack));
                NumericValue right = parse_value(evaluate_recursive(opNode->getRight(), astRoot, contextNode, call_stack));

                switch (opNode->getOp()) {
                    case TokenType::PLUS:
                        if (left.unit != right.unit) throw std::runtime_error("Mismatched units for addition.");
                        result_str = format_value({left.value + right.value, left.unit});
                        break;
                    case TokenType::MINUS:
                        if (left.unit != right.unit) throw std::runtime_error("Mismatched units for subtraction.");
                        result_str = format_value({left.value - right.value, left.unit});
                        break;
                    case TokenType::STAR:
                        if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot multiply two values with units.");
                        result_str = format_value({left.value * right.value, left.unit.empty() ? right.unit : left.unit});
                        break;
                    case TokenType::SLASH:
                        if (right.value == 0) throw std::runtime_error("Division by zero.");
                        if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot divide two values with units.");
                        result_str = format_value({left.value / right.value, left.unit.empty() ? right.unit : left.unit});
                        break;
                    // Comparison operators
                    case TokenType::GREATER: result_str = (left.value > right.value) ? "1" : "0"; break;
                    case TokenType::LESS: result_str = (left.value < right.value) ? "1" : "0"; break;
                    case TokenType::GREATER_EQUAL: result_str = (left.value >= right.value) ? "1" : "0"; break;
                    case TokenType::LESS_EQUAL: result_str = (left.value <= right.value) ? "1" : "0"; break;
                    case TokenType::EQUAL_EQUAL: result_str = (left.value == right.value) ? "1" : "0"; break;
                    case TokenType::NOT_EQUAL: result_str = (left.value != right.value) ? "1" : "0"; break;
                    // Logical operators
                    case TokenType::LOGICAL_AND: result_str = (left.value != 0 && right.value != 0) ? "1" : "0"; break;
                    case TokenType::LOGICAL_OR: result_str = (left.value != 0 || right.value != 0) ? "1" : "0"; break;
                    default: throw std::runtime_error("Unsupported binary operator.");
                }
                break;
            }
            case NodeType::Conditional: {
                const auto* condNode = static_cast<const ConditionalNode*>(node);
                std::string cond_result_str = evaluate_recursive(condNode->getCondition(), astRoot, contextNode, call_stack);
                NumericValue cond_val = parse_value(cond_result_str);
                if (cond_val.value != 0) {
                    result_str = evaluate_recursive(condNode->getTrueExpression(), astRoot, contextNode, call_stack);
                } else {
                    result_str = evaluate_recursive(condNode->getFalseExpression(), astRoot, contextNode, call_stack);
                }
                break;
            }
            case NodeType::Reference: {
                const auto* refNode = static_cast<const ReferenceNode*>(node);
                const std::string& refStr = refNode->getReference();
                size_t last_dot = refStr.find_last_of('.');
                const ElementNode* targetNode = nullptr;
                std::string propertyName;

                if (last_dot == std::string::npos) { // Self-reference
                   if (!contextNode || contextNode->getType() != NodeType::Element) {
                        throw std::runtime_error("Cannot resolve self-reference: no valid element context provided.");
                   }
                   targetNode = static_cast<const ElementNode*>(contextNode);
                   propertyName = refStr;
                } else { // Reference to another node
                   std::string selector = refStr.substr(0, last_dot);
                   propertyName = refStr.substr(last_dot + 1);
                   targetNode = find_node_by_selector(selector, astRoot);
                   if (!targetNode) throw std::runtime_error("Reference error: selector '" + selector + "' not found.");
                }

                const ExpressionNode* propertyExpr = find_property_in_node(targetNode, propertyName);
                if (!propertyExpr) throw std::runtime_error("Reference error: property '" + propertyName + "' not found on the specified element.");

                result_str = evaluate_recursive(propertyExpr, astRoot, contextNode, call_stack);
                break;
            }
            default:
                throw std::runtime_error("Unknown or unsupported expression node type for evaluation.");
        }
        call_stack.pop_back();
        return result_str;
    }
}

std::string ExpressionEvaluator::evaluate(const ExpressionNode* node, const BaseNode* astRoot, const BaseNode* contextNode) {
    std::vector<const ExpressionNode*> call_stack;
    return evaluate_recursive(node, astRoot, contextNode, call_stack);
}