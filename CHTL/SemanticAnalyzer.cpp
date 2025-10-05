#include "SemanticAnalyzer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/IfNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/NumericLiteralNode.h"
#include "CHTLNode/ReferenceNode.h"
#include "CHTLNode/BooleanLiteralNode.h"
#include "Common/Value.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>

namespace CHTL {

// Helper to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Helper to extract numeric value from a string like "100px"
double extractNumericValue(const std::string& str) {
    std::string num_str;
    for (char c : str) {
        if (std::isdigit(c) || c == '.') {
            num_str += c;
        }
    }
    return num_str.empty() ? 0.0 : std::stod(num_str);
}

void SemanticAnalyzer::checkNodeAgainstConstraints(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& constraints) {
    if (!node || node->getType() == NodeType::NODE_CONSTRAINT) return;

    for (const auto& constraint : constraints) {
        bool violation = false;
        std::string errorMsg;

        switch (constraint.type) {
            case ConstraintType::HTML_TAG:
                if (node->getType() == NodeType::NODE_ELEMENT && std::dynamic_pointer_cast<ElementNode>(node)->getTagName() == constraint.value) {
                    violation = true;
                    errorMsg = "Constraint violation: HTML tag <" + constraint.value + "> is not allowed here.";
                }
                break;
            case ConstraintType::ANY_HTML_TAG:
                if (node->getType() == NodeType::NODE_ELEMENT) {
                    violation = true;
                    errorMsg = "Constraint violation: HTML tags are not allowed here.";
                }
                break;
            case ConstraintType::NAMED_TEMPLATE:
                 if (node->getType() == NodeType::NODE_TEMPLATE_USAGE) {
                    auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node);
                    if (usageNode->getName() == constraint.value) {
                        violation = true;
                        errorMsg = "Constraint violation: Template <" + constraint.value + "> is not allowed here.";
                    }
                }
                break;
            case ConstraintType::ANY_TEMPLATE:
                if (node->getType() == NodeType::NODE_TEMPLATE_USAGE) {
                    violation = true;
                    errorMsg = "Constraint violation: Templates are not allowed here.";
                }
                break;
            default:
                break;
        }

        if (violation) {
            throw std::runtime_error(errorMsg);
        }
    }
}

bool SemanticAnalyzer::evaluateCondition(const std::shared_ptr<BaseNode>& condition, const std::shared_ptr<ElementNode>& scope) {
    Value result = evaluateExpression(condition, scope);
    if (result.type != Value::Type::BOOL) {
        throw std::runtime_error("Condition must evaluate to a boolean value.");
    }
    return std::get<bool>(result.value);
}

Value SemanticAnalyzer::evaluateExpression(const std::shared_ptr<BaseNode>& node, const std::shared_ptr<ElementNode>& scope) {
    if (!node) {
        throw std::runtime_error("Cannot evaluate a null expression node.");
    }

    if (node->getType() == NodeType::NODE_BOOLEAN_LITERAL) {
        auto boolNode = std::dynamic_pointer_cast<BooleanLiteralNode>(node);
        return { Value::Type::BOOL, boolNode->value };
    }

    if (node->getType() == NodeType::NODE_NUMERIC_LITERAL) {
        auto numNode = std::dynamic_pointer_cast<NumericLiteralNode>(node);
        return { Value::Type::NUMBER, std::stod(numNode->value), numNode->unit };
    }

    if (node->getType() == NodeType::NODE_REFERENCE) {
        auto refNode = std::dynamic_pointer_cast<ReferenceNode>(node);
        // This is a simplified implementation. A real implementation would
        // involve a more robust search of the AST.
        if (scope && scope->hasAttribute(refNode->propertyName)) {
            std::string attrValue = scope->getAttributes().at(refNode->propertyName);
            return { Value::Type::NUMBER, extractNumericValue(attrValue), ""};
        }
        throw std::runtime_error("Unable to resolve reference: " + refNode->selector + "." + refNode->propertyName);
    }

    if (node->getType() == NodeType::NODE_TEMPLATE_USAGE) {
        auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node);
        if (usageNode && usageNode->getUsageType() == TemplateUsageType::VAR) {
            if (!context) {
                throw std::runtime_error("GenerationContext not available for template lookup.");
            }
            auto templateNode = context->getTemplate(usageNode->getName());
            if (templateNode) {
                for (const auto& varChild : templateNode->getChildren()) {
                    if (varChild->getType() == NodeType::NODE_PROPERTY) {
                        auto varProp = std::dynamic_pointer_cast<PropertyNode>(varChild);
                        if (varProp && varProp->getKey() == usageNode->getVariableName()) {
                             std::string value = varProp->getValue();
                             std::string value_str;
                             std::string unit_str;
                             size_t i = 0;
                             while (i < value.length() && (std::isdigit(value[i]) || value[i] == '.')) {
                                 value_str += value[i];
                                 i++;
                             }
                             while (i < value.length()) {
                                 unit_str += value[i];
                                 i++;
                             }
                             return { Value::Type::NUMBER, std::stod(value_str), unit_str };
                        }
                    }
                }
            }
        }
        throw std::runtime_error("Failed to resolve template variable in expression.");
    }

    if (node->getType() == NodeType::NODE_BINARY_OP) {
        auto opNode = std::dynamic_pointer_cast<BinaryOpNode>(node);
        auto left = evaluateExpression(opNode->left, scope);
        auto right = evaluateExpression(opNode->right, scope);

        if (opNode->op == "+") {
            if (left.type != Value::Type::NUMBER || right.type != Value::Type::NUMBER) throw std::runtime_error("Addition requires numeric operands.");
            return { Value::Type::NUMBER, std::get<double>(left.value) + std::get<double>(right.value), left.unit };
        }
        if (opNode->op == "-") {
            if (left.type != Value::Type::NUMBER || right.type != Value::Type::NUMBER) throw std::runtime_error("Subtraction requires numeric operands.");
            return { Value::Type::NUMBER, std::get<double>(left.value) - std::get<double>(right.value), left.unit };
        }
        if (opNode->op == "*") {
            if (left.type != Value::Type::NUMBER || right.type != Value::Type::NUMBER) throw std::runtime_error("Multiplication requires numeric operands.");
            return { Value::Type::NUMBER, std::get<double>(left.value) * std::get<double>(right.value), left.unit };
        }
        if (opNode->op == "/") {
            if (left.type != Value::Type::NUMBER || right.type != Value::Type::NUMBER) throw std::runtime_error("Division requires numeric operands.");
            if (std::get<double>(right.value) == 0) throw std::runtime_error("Division by zero.");
            return { Value::Type::NUMBER, std::get<double>(left.value) / std::get<double>(right.value), left.unit };
        }
        if (opNode->op == ">") {
            if (left.type != Value::Type::NUMBER || right.type != Value::Type::NUMBER) throw std::runtime_error("Comparison requires numeric operands.");
            return { Value::Type::BOOL, std::get<double>(left.value) > std::get<double>(right.value) };
        }
        if (opNode->op == "<") {
            if (left.type != Value::Type::NUMBER || right.type != Value::Type::NUMBER) throw std::runtime_error("Comparison requires numeric operands.");
            return { Value::Type::BOOL, std::get<double>(left.value) < std::get<double>(right.value) };
        }
        if (opNode->op == "==") {
             if (left.type != right.type) return { Value::Type::BOOL, false };
             return { Value::Type::BOOL, left.value == right.value };
        }
        if (opNode->op == "&&") {
            if (left.type != Value::Type::BOOL || right.type != Value::Type::BOOL) throw std::runtime_error("Logical AND requires boolean operands.");
            return { Value::Type::BOOL, std::get<bool>(left.value) && std::get<bool>(right.value) };
        }
        if (opNode->op == "||") {
            if (left.type != Value::Type::BOOL || right.type != Value::Type::BOOL) throw std::runtime_error("Logical OR requires boolean operands.");
            return { Value::Type::BOOL, std::get<bool>(left.value) || std::get<bool>(right.value) };
        }

        throw std::runtime_error("Unsupported operator in expression: " + opNode->op);
    }

    throw std::runtime_error("Unsupported node type in expression evaluation.");
}

void SemanticAnalyzer::analyze(std::shared_ptr<BaseNode>& root, const GenerationContext& context) {
    this->context = &context;
    this->root = root;
    if (root) {
        visit(root, {}, nullptr);
    }
}

void SemanticAnalyzer::visit(std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints, std::shared_ptr<ElementNode> parent) {
    if (!node) return;

    checkNodeAgainstConstraints(node, active_constraints);

    if (node->getType() == NodeType::NODE_ELEMENT) {
        visitElement(std::dynamic_pointer_cast<ElementNode>(node), active_constraints);
    } else if (node->getType() == NodeType::NODE_STYLE) {
        visitStyleNode(std::dynamic_pointer_cast<StyleNode>(node), parent);
    } else {
        // We recursively visit children for all other node types.
        // Note that if-nodes are handled within visitStyleNode and visitElement.
        auto& children = node->getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            visit(children[i], active_constraints, parent);
        }
    }
}

void SemanticAnalyzer::visitStyleNode(std::shared_ptr<StyleNode> node, std::shared_ptr<ElementNode> parent) {
    auto old_children = node->getChildren();
    std::vector<std::shared_ptr<BaseNode>> new_children;

    for (size_t i = 0; i < old_children.size(); ++i) {
        auto child = old_children[i];

        if (child->getType() == NodeType::NODE_IF) {
            auto if_node = std::dynamic_pointer_cast<IfNode>(child);
            if (if_node->if_type != IfType::IF) {
                continue; // Only start processing from an 'if'
            }

            bool condition_met_in_chain = false;

            auto process_and_add_props = [&](const std::shared_ptr<BaseNode>& container) {
                for (const auto& prop_child : container->getChildren()) {
                    if (prop_child->getType() == NodeType::NODE_PROPERTY) {
                        auto propNode = std::dynamic_pointer_cast<PropertyNode>(prop_child);
                        if (propNode && !propNode->getChildren().empty()) {
                            auto expression_root = propNode->getChildren()[0];
                            Value result = evaluateExpression(expression_root, parent);
                            if (result.type == Value::Type::NUMBER) {
                                propNode->setValue(std::to_string(std::get<double>(result.value)) + result.unit);
                            }
                             else if (result.type == Value::Type::STRING) {
                                propNode->setValue(std::get<std::string>(result.value));
                            }
                            propNode->setChildren({});
                        }
                        new_children.push_back(propNode);
                    } else {
                        new_children.push_back(prop_child);
                    }
                }
            };

            // Process the main 'if'
            if (evaluateCondition(if_node->condition, parent)) {
                condition_met_in_chain = true;
                process_and_add_props(if_node);
            }

            // Process 'else if' and 'else' chain
            while (i + 1 < old_children.size()) {
                auto next_child = old_children[i + 1];
                if (next_child->getType() != NodeType::NODE_IF) break;

                auto next_if_node = std::dynamic_pointer_cast<IfNode>(next_child);

                if (next_if_node->if_type == IfType::ELSE_IF) {
                    i++; // Consume the 'else if'
                    if (!condition_met_in_chain && evaluateCondition(next_if_node->condition, parent)) {
                        condition_met_in_chain = true;
                        process_and_add_props(next_if_node);
                    }
                } else if (next_if_node->if_type == IfType::ELSE) {
                    i++; // Consume the 'else'
                    if (!condition_met_in_chain) {
                        process_and_add_props(next_if_node);
                    }
                    break;
                } else {
                    break;
                }
            }
        } else if (child->getType() == NodeType::NODE_PROPERTY) {
            auto propNode = std::dynamic_pointer_cast<PropertyNode>(child);
            if (propNode && !propNode->getChildren().empty()) {
                auto expression_root = propNode->getChildren()[0];
                Value result = evaluateExpression(expression_root, parent);
                 if (result.type == Value::Type::NUMBER) {
                    propNode->setValue(std::to_string(std::get<double>(result.value)) + result.unit);
                }
                else if (result.type == Value::Type::STRING) {
                    propNode->setValue(std::get<std::string>(result.value));
                }
                propNode->setChildren({});
            }
            new_children.push_back(propNode);
        }
        else {
            new_children.push_back(child);
        }
    }
    node->setChildren(new_children);
}

void SemanticAnalyzer::visitElement(std::shared_ptr<ElementNode> node, const std::vector<Constraint>& active_constraints) {
    auto current_constraints = active_constraints;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_CONSTRAINT) {
            auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(child);
            for (const auto& c : constraintNode->constraints) {
                current_constraints.push_back(c);
            }
        }
    }

    std::shared_ptr<StyleNode> styleNode = nullptr;
    std::vector<std::shared_ptr<BaseNode>> new_element_children;
    bool has_direct_if_children = false;

    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_IF) {
            has_direct_if_children = true;
        }
        if (child->getType() == NodeType::NODE_STYLE) {
            styleNode = std::dynamic_pointer_cast<StyleNode>(child);
        }
    }

    if (has_direct_if_children) {
        if (!styleNode) {
            styleNode = std::make_shared<StyleNode>();
        }
        for (const auto& child : node->getChildren()) {
            if (child->getType() == NodeType::NODE_IF) {
                styleNode->addChild(child);
            } else if (child->getType() != NodeType::NODE_STYLE) {
                new_element_children.push_back(child);
            }
        }
        new_element_children.push_back(styleNode);
        node->setChildren(new_element_children);
    }

    auto& children = node->getChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        visit(children[i], current_constraints, node);
    }
}

}