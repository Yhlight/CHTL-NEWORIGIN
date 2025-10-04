#include "TemplateEvaluator.h"
#include "../../CHTL/CHTLNode/TemplateDefinitionNode.h"
#include "../../CHTL/CHTLNode/TemplateUsageNode.h"
#include <vector>
#include <algorithm>

void TemplateEvaluator::evaluate(BaseNode* root) {
    if (!root) return;
    findDefinitions(root);
    expandTemplates(root);
}

void TemplateEvaluator::findDefinitions(BaseNode* node) {
    if (!node) return;

    if (node->getType() == NodeType::TemplateDefinition) {
        auto* defNode = static_cast<TemplateDefinitionNode*>(node);
        if (defNode->getTemplateType() == "Style") {
            styleTemplates[defNode->getTemplateName()] = defNode;
        }
    }

    for (const auto& child : node->getChildren()) {
        findDefinitions(child.get());
    }
}

void TemplateEvaluator::expandTemplates(BaseNode* node) {
    if (!node) return;

    // Move children to a temporary vector to safely iterate for recursion.
    auto childrenToProcess = std::move(node->getChildren());
    node->getChildren().clear();

    // First, recurse on the original children to handle nested templates.
    for (const auto& child : childrenToProcess) {
        expandTemplates(child.get());
    }

    // Now, expand templates at the current level.
    for (auto& child : childrenToProcess) {
        if (child && child->getType() == NodeType::TemplateUsage) {
            auto* usageNode = static_cast<TemplateUsageNode*>(child.get());

            if (usageNode->getTemplateType() == "Style") {
                auto it = styleTemplates.find(usageNode->getTemplateName());
                if (it != styleTemplates.end()) {
                    TemplateDefinitionNode* defNode = it->second;
                    for (const auto& prop : defNode->getChildren()) {
                        node->addChild(prop->clone());
                    }
                }
                // If template is not found, usage node is silently discarded.
            } else {
                // For other template types, just move it back for now.
                node->addChild(std::move(child));
            }
        } else if (child) {
            // Not a usage node, so just move it back to the parent.
            node->addChild(std::move(child));
        }
    }
}