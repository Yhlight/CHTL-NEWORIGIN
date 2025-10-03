#include "TemplateEvaluator.h"
#include "../../CHTL/CHTLNode/TemplateDefinitionNode.h"
#include "../../CHTL/CHTLNode/CustomDefinitionNode.h"
#include "../../CHTL/CHTLNode/TemplateUsageNode.h"
#include "../../CHTL/CHTLNode/CustomUsageNode.h"
#include <vector>
#include <iostream>

void TemplateEvaluator::evaluate(BaseNode* root) {
    if (!root) return;
    this->astRoot = root;

    // First, find all definitions.
    findDefinitions(root);
    // Then, expand all usages.
    processNode(root);
}

void TemplateEvaluator::findDefinitions(BaseNode* node) {
    if (!node) return;

    if (node->getType() == NodeType::TemplateDefinition) {
        auto* defNode = static_cast<TemplateDefinitionNode*>(node);
        std::string key = defNode->getTemplateType() + "::" + defNode->getTemplateName();
        templateDefinitions[key] = defNode;
    } else if (node->getType() == NodeType::CustomDefinition) {
        auto* defNode = static_cast<CustomDefinitionNode*>(node);
        std::string key = defNode->getCustomType() + "::" + defNode->getCustomName();
        customDefinitions[key] = defNode;
    }

    // Recurse to find all definitions
    for (const auto& child : node->getChildren()) {
        findDefinitions(child.get());
    }
}

void TemplateEvaluator::processNode(BaseNode* node) {
    if (!node) return;

    // Recurse to the bottom of the tree first (post-order traversal).
    for (auto& child : node->getMutableChildren()) {
        processNode(child.get());
    }

    // Now process the children of the current node.
    auto originalChildren = std::move(node->getMutableChildren());
    node->getMutableChildren().clear();

    for (auto& child : originalChildren) {
        if (child && child->getType() == NodeType::TemplateUsage) {
            auto* usageNode = static_cast<TemplateUsageNode*>(child.get());
            std::string key = usageNode->getTemplateType() + "::" + usageNode->getTemplateName();
            if (templateDefinitions.count(key)) {
                const auto* defNode = templateDefinitions[key];
                for (const auto& defChild : defNode->getChildren()) {
                    node->addChild(defChild->clone());
                }
            }
        } else if (child && child->getType() == NodeType::CustomUsage) {
            auto* usageNode = static_cast<CustomUsageNode*>(child.get());
            std::string key = usageNode->getCustomType() + "::" + usageNode->getCustomName();
            if (customDefinitions.count(key)) {
                const auto* defNode = customDefinitions[key];
                for (const auto& defChild : defNode->getChildren()) {
                    node->addChild(defChild->clone());
                }
            }
        } else {
            node->addChild(std::move(child));
        }
    }
}