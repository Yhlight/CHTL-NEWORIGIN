#include "SemanticAnalyzer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/CustomNode.h"
#include <stdexcept>
#include <iostream>

namespace CHTL {

// Helper function to check a node against a list of constraints
void checkNodeAgainstConstraints(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& constraints) {
    if (!node) return;

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
            case ConstraintType::TEMPLATE_SUBTYPE:
                if (node->getType() == NodeType::NODE_TEMPLATE_USAGE) {
                    auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node);
                    // This check assumes you have a way to get the subtype of a template usage.
                    // For now, we'll assume a naming convention or a more complex lookup is needed.
                    // This part might need further refinement based on how template subtypes are managed.
                }
                break;
            // Add cases for CUSTOM constraints here as CustomNode becomes more defined
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
        visit(root, {});
    }
}

void SemanticAnalyzer::visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints) {
    if (!node) return;

    // Check the current node against active constraints before processing its children.
    checkNodeAgainstConstraints(node, active_constraints);

    // If the node is an element, it might define new constraints for its children.
    if (node->getType() == NodeType::NODE_ELEMENT) {
        visitElement(std::dynamic_pointer_cast<ElementNode>(node), active_constraints);
    } else {
        // For other nodes, just continue traversal with the same constraints.
        for (const auto& child : node->getChildren()) {
            visit(child, active_constraints);
        }
    }
}

void SemanticAnalyzer::visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints) {
    // Gather new constraints from the current element's children.
    auto current_constraints = active_constraints;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_CONSTRAINT) {
            auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(child);
            for (const auto& c : constraintNode->constraints) {
                current_constraints.push_back(c);
            }
        }
    }

    // Visit all children of the element, passing the updated set of constraints.
    for (const auto& child : node->getChildren()) {
        // We don't check constraint nodes themselves, only other node types.
        if (child->getType() != NodeType::NODE_CONSTRAINT) {
            visit(child, current_constraints);
        }
    }
}

}