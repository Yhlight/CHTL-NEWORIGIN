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

void SemanticAnalyzer::analyze(const std::shared_ptr<BaseNode>& root) {
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
        // For other node types that might have children, continue traversal.
        for (const auto& child : node->getChildren()) {
            visit(child, active_constraints, parent);
        }
    }
}

void SemanticAnalyzer::visitStyleNode(const std::shared_ptr<StyleNode>& node, const std::shared_ptr<ElementNode>& parent) {
    // The static evaluation logic has been removed.
    // The generator will now handle IfNode.
    // We still need to traverse the children of the style node.
    for (const auto& child : node->getChildren()) {
        visit(child, {}, parent);
    }
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

    for (const auto& child : node->getChildren()) {
        visit(child, current_constraints, node);
    }
}

}