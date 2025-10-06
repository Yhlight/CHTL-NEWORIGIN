#include "SemanticAnalyzer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/IfNode.h"
#include "CHTLNode/PropertyNode.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

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

// Helper to find a node by a simple tag selector
std::shared_ptr<ElementNode> findNodeBySelector(const std::shared_ptr<BaseNode>& root, const std::string& selector) {
    if (!root) return nullptr;

    if (root->getType() == NodeType::NODE_ELEMENT) {
        auto elementNode = std::dynamic_pointer_cast<ElementNode>(root);
        if (elementNode->getTagName() == selector) {
            return elementNode;
        }
    }

    for (const auto& child : root->getChildren()) {
        auto found = findNodeBySelector(child, selector);
        if (found) return found;
    }

    return nullptr;
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

bool SemanticAnalyzer::evaluateCondition(const std::string& condition, const std::shared_ptr<ElementNode>& scope, const std::shared_ptr<BaseNode>& root) {
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
    size_t dot_pos = lhs_str.find('.');
    if (dot_pos != std::string::npos) {
        std::string selector = lhs_str.substr(0, dot_pos);
        std::string prop = lhs_str.substr(dot_pos + 1);
        auto target_node = findNodeBySelector(root, selector);
        if (target_node && target_node->hasAttribute(prop)) {
            lhs_val = extractNumericValue(target_node->getAttributes().at(prop));
        }
    } else {
        if (scope && scope->hasAttribute(lhs_str)) {
            lhs_val = extractNumericValue(scope->getAttributes().at(lhs_str));
        } else {
            lhs_val = extractNumericValue(lhs_str);
        }
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

void SemanticAnalyzer::analyze(const std::shared_ptr<BaseNode>& root) {
    if (root) {
        visit(root, {}, nullptr, root);
    }
}

void SemanticAnalyzer::visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints, const std::shared_ptr<ElementNode>& parent, const std::shared_ptr<BaseNode>& root) {
    if (!node) return;

    checkNodeAgainstConstraints(node, active_constraints);

    if (node->getType() == NodeType::NODE_ELEMENT) {
        visitElement(std::dynamic_pointer_cast<ElementNode>(node), active_constraints, root);
    } else if (node->getType() == NodeType::NODE_STYLE) {
        visitStyleNode(std::dynamic_pointer_cast<StyleNode>(node), parent, root);
    } else {
        // For other node types that might have children, continue traversal.
        for (const auto& child : node->getChildren()) {
            visit(child, active_constraints, parent, root);
        }
    }
}

void SemanticAnalyzer::visitStyleNode(const std::shared_ptr<StyleNode>& node, const std::shared_ptr<ElementNode>& parent, const std::shared_ptr<BaseNode>& root) {
    std::vector<std::shared_ptr<BaseNode>> new_children;

    for (size_t i = 0; i < node->getChildren().size(); ++i) {
        auto child = node->getChildren()[i];

        if (child->getType() == NodeType::NODE_IF) {
            auto if_node = std::dynamic_pointer_cast<IfNode>(child);
            if (if_node->isDynamic) {
                new_children.push_back(child);
                // Also keep subsequent else-if/else blocks if they are part of this dynamic chain
                while (i + 1 < node->getChildren().size()) {
                    auto next_child = node->getChildren()[i + 1];
                    if (auto next_if = std::dynamic_pointer_cast<IfNode>(next_child)) {
                        if (next_if->if_type == IfType::ELSE_IF || next_if->if_type == IfType::ELSE) {
                            new_children.push_back(next_child);
                            i++;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                continue;
            }

            if (if_node->if_type == IfType::IF) {
                bool condition_met = false;
                if (evaluateCondition(if_node->condition, parent, root)) {
                    condition_met = true;
                    for (const auto& c : if_node->getChildren()) new_children.push_back(c);
                }

                if (!condition_met) {
                     while (i + 1 < node->getChildren().size()) {
                        auto next_child = node->getChildren()[i + 1];
                        if (next_child->getType() == NodeType::NODE_IF) {
                            auto next_if = std::dynamic_pointer_cast<IfNode>(next_child);
                            if (next_if->if_type == IfType::ELSE_IF) {
                                i++;
                                if (evaluateCondition(next_if->condition, parent, root)) {
                                    condition_met = true;
                                    for (const auto& c : next_if->getChildren()) new_children.push_back(c);
                                    break;
                                }
                            } else if (next_if->if_type == IfType::ELSE) {
                                i++;
                                for (const auto& c : next_if->getChildren()) new_children.push_back(c);
                                break;
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }
                }
            }
        } else {
            new_children.push_back(child);
        }
    }
    node->setChildren(new_children);
}

void SemanticAnalyzer::visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints, const std::shared_ptr<BaseNode>& root) {
    auto current_constraints = active_constraints;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_CONSTRAINT) {
            auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(child);
            for (const auto& c : constraintNode->constraints) {
                current_constraints.push_back(c);
            }
        }
    }

    for (const auto& child : node->getChildren()) {
        visit(child, current_constraints, node, root);
    }
}

}