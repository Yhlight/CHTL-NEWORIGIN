#include "SemanticAnalyzer.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include <stdexcept>

namespace CHTL {

void SemanticAnalyzer::analyze(const std::shared_ptr<BaseNode>& root) {
    if (root) {
        visit(root, {});
    }
}

void SemanticAnalyzer::visit(const std::shared_ptr<BaseNode>& node, const std::vector<Constraint>& active_constraints) {
    if (!node) return;

    if (node->getType() == NodeType::NODE_ELEMENT) {
        visitElement(std::dynamic_pointer_cast<ElementNode>(node), active_constraints);
    } else {
        // For non-element nodes, just continue traversal
        for (const auto& child : node->getChildren()) {
            visit(child, active_constraints);
        }
    }
}

void SemanticAnalyzer::visitElement(const std::shared_ptr<ElementNode>& node, const std::vector<Constraint>& active_constraints) {
    // 1. Check if the current element violates any active constraints
    for (const auto& constraint : active_constraints) {
        if (constraint.type == ConstraintType::TAG_NAME && node->getTagName() == constraint.value) {
            throw std::runtime_error("Constraint violation: Element <" + node->getTagName() + "> is not allowed here.");
        }
    }

    // 2. Gather new constraints from the current element
    auto current_constraints = active_constraints;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_CONSTRAINT) {
            auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(child);
            for (const auto& c : constraintNode->constraints) {
                current_constraints.push_back(c);
            }
        }
    }

    // 3. Visit children with the updated set of constraints
    for (const auto& child : node->getChildren()) {
        // Also check TemplateUsageNode against constraints
        if(child->getType() == NodeType::NODE_TEMPLATE_USAGE) {
            auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(child);
            for (const auto& constraint : current_constraints) {
                 if (constraint.type == ConstraintType::TYPE_TEMPLATE && usageNode->getUsageType() == TemplateUsageType::ELEMENT) {
                     throw std::runtime_error("Constraint violation: Template elements are not allowed here.");
                 }
                 if (constraint.type == ConstraintType::TEMPLATE && usageNode->getUsageType() == TemplateUsageType::ELEMENT && usageNode->getName() == constraint.value) {
                     throw std::runtime_error("Constraint violation: Template <" + usageNode->getName() + "> is not allowed here.");
                 }
            }
        }
        visit(child, current_constraints);
    }
}

}