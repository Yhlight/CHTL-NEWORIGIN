#include "SemanticAnalyzer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/IfNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/NumericLiteralNode.h"
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

bool SemanticAnalyzer::evaluateCondition(const std::string& condition, const std::shared_ptr<ElementNode>& scope) {
    std::string trimmed_condition = trim(condition);
    if (trimmed_condition == "true") return true;
    if (trimmed_condition == "false") return false;

    std::istringstream iss(condition);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.size() != 3) {
        throw std::runtime_error("Unsupported condition format: " + condition);
    }

    std::string lhs_str = tokens[0];
    std::string op = tokens[1];
    std::string rhs_str = tokens[2];

    double lhs_val = 0.0;
    if (scope && scope->hasAttribute(lhs_str)) {
        lhs_val = extractNumericValue(scope->getAttributes().at(lhs_str));
    } else {
        lhs_val = extractNumericValue(lhs_str);
    }

    double rhs_val = extractNumericValue(rhs_str);

    if (op == ">") return lhs_val > rhs_val;
    if (op == "<") return lhs_val < rhs_val;
    if (op == ">=") return lhs_val >= rhs_val;
    if (op == "<=") return lhs_val <= rhs_val;
    if (op == "==") return lhs_val == rhs_val;
    if (op == "!=") return lhs_val != rhs_val;

    throw std::runtime_error("Unsupported operator in condition: " + op);
}

std::shared_ptr<NumericLiteralNode> SemanticAnalyzer::evaluateExpression(const std::shared_ptr<BaseNode>& node) {
    if (!node) {
        throw std::runtime_error("Cannot evaluate a null expression node.");
    }

    if (node->getType() == NodeType::NODE_NUMERIC_LITERAL) {
        return std::dynamic_pointer_cast<NumericLiteralNode>(node);
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
                             return std::make_shared<NumericLiteralNode>(value_str, unit_str);
                        }
                    }
                }
            }
        }
        throw std::runtime_error("Failed to resolve template variable in expression.");
    }

    if (node->getType() == NodeType::NODE_BINARY_OP) {
        auto opNode = std::dynamic_pointer_cast<BinaryOpNode>(node);
        auto left = evaluateExpression(opNode->left);
        auto right = evaluateExpression(opNode->right);

        double left_val = std::stod(left->value);
        double right_val = std::stod(right->value);
        double result_val = 0.0;

        std::string result_unit = !left->unit.empty() ? left->unit : right->unit;
        if (!left->unit.empty() && !right->unit.empty() && left->unit != right->unit) {
            throw std::runtime_error("Unit mismatch in expression: " + left->unit + " and " + right->unit);
        }

        if (opNode->op == "+") result_val = left_val + right_val;
        else if (opNode->op == "-") result_val = left_val - right_val;
        else if (opNode->op == "*") result_val = left_val * right_val;
        else if (opNode->op == "/") {
            if (right_val == 0) throw std::runtime_error("Division by zero in expression.");
            result_val = left_val / right_val;
        }
        else if (opNode->op == "%") result_val = fmod(left_val, right_val);
        else if (opNode->op == "**") result_val = pow(left_val, right_val);
        else {
            throw std::runtime_error("Unsupported operator in expression: " + opNode->op);
        }

        return std::make_shared<NumericLiteralNode>(std::to_string(result_val), result_unit);
    }

    throw std::runtime_error("Unsupported node type in expression evaluation.");
}

void SemanticAnalyzer::analyze(const std::shared_ptr<BaseNode>& root, const GenerationContext& context) {
    this->context = &context;
    if (root) {
        visit(root, {}, nullptr);
    }
}

void SemanticAnalyzer::visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints, const std::shared_ptr<ElementNode>& parent) {
    if (!node) return;

    checkNodeAgainstConstraints(node, active_constraints);

    if (node->getType() == NodeType::NODE_ELEMENT) {
        visitElement(std::dynamic_pointer_cast<ElementNode>(node), active_constraints);
    } else if (node->getType() == NodeType::NODE_STYLE) {
        visitStyleNode(std::dynamic_pointer_cast<StyleNode>(node), parent);
    } else {
        for (const auto& child : node->getChildren()) {
            visit(child, active_constraints, parent);
        }
    }
}

void SemanticAnalyzer::visitStyleNode(const std::shared_ptr<StyleNode>& node, const std::shared_ptr<ElementNode>& parent) {
    auto old_children = node->getChildren();
    std::vector<std::shared_ptr<BaseNode>> new_children;

    for (size_t i = 0; i < old_children.size(); ++i) {
        auto child = old_children[i];

        if (child->getType() == NodeType::NODE_IF) {
            auto if_node = std::dynamic_pointer_cast<IfNode>(child);
            if (if_node->if_type != IfType::IF) {
                continue;
            }

            bool condition_met_in_chain = false;

            if (evaluateCondition(if_node->condition, parent)) {
                condition_met_in_chain = true;
                for (const auto& prop : if_node->getChildren()) {
                    new_children.push_back(prop);
                }
            }

            while (i + 1 < old_children.size()) {
                auto next_child = old_children[i + 1];
                if (next_child->getType() != NodeType::NODE_IF) break;

                auto next_if_node = std::dynamic_pointer_cast<IfNode>(next_child);

                if (next_if_node->if_type == IfType::ELSE_IF) {
                    i++;
                    if (!condition_met_in_chain && evaluateCondition(next_if_node->condition, parent)) {
                        condition_met_in_chain = true;
                        for (const auto& prop : next_if_node->getChildren()) {
                            new_children.push_back(prop);
                        }
                    }
                } else if (next_if_node->if_type == IfType::ELSE) {
                    i++;
                    if (!condition_met_in_chain) {
                        for (const auto& prop : next_if_node->getChildren()) {
                            new_children.push_back(prop);
                        }
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
                auto result_node = evaluateExpression(expression_root);
                propNode->setValue(result_node->value + result_node->unit);
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

void SemanticAnalyzer::visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints) {
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

    for (const auto& child : node->getChildren()) {
        visit(child, current_constraints, node);
    }
}

}