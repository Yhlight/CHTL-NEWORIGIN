#include "CHTLResolver.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ElementSpecializationNode.h"
#include "../CHTLNode/DeleteElementNode.h"
#include <iostream>
#include <algorithm>

// A simple clone function for nodes that don't have children to resolve
template <typename T>
std::shared_ptr<T> cloneNode(const std::shared_ptr<BaseNode>& node) {
    auto original = std::dynamic_pointer_cast<T>(node);
    return std::make_shared<T>(*original);
}

CHTLResolver::CHTLResolver(CHTLContext& context) : context(context) {}

std::shared_ptr<ElementNode> CHTLResolver::resolve(const std::shared_ptr<ElementNode>& root) {
    if (!root) return nullptr;
    // The top-level resolve call should return a single element node
    auto resolved_nodes = resolveNode(root);
    if (resolved_nodes.empty()) return nullptr;
    return std::dynamic_pointer_cast<ElementNode>(resolved_nodes[0]);
}

std::vector<std::shared_ptr<BaseNode>> CHTLResolver::resolveNode(const std::shared_ptr<BaseNode>& node) {
    if (!node) return {};

    switch (node->getType()) {
        case NodeType::Element: {
            return { resolveElement(std::dynamic_pointer_cast<ElementNode>(node)) };
        }
        case NodeType::TemplateUsage: {
            auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node);
            if (usageNode->templateType == TemplateType::Element) {
                std::vector<std::shared_ptr<BaseNode>> childrenToProcess;
                bool found = false;

                // Get the initial list of children from the definition
                if (context.elementTemplates.count(usageNode->name)) {
                    childrenToProcess = context.elementTemplates[usageNode->name]->children;
                    found = true;
                } else if (context.elementCustoms.count(usageNode->name)) {
                    childrenToProcess = context.elementCustoms[usageNode->name]->children;
                    found = true;
                }

                if (!found) {
                     std::cerr << "Warning: Undefined element template or custom used: " << usageNode->name << std::endl;
                     return {};
                }

                // Apply specializations if they exist
                if (usageNode->specialization) {
                    for (const auto& instruction : usageNode->specialization->instructions) {
                        if (instruction->getType() == NodeType::DeleteElement) {
                            auto deleteInstruction = std::dynamic_pointer_cast<DeleteElementNode>(instruction);
                            // Erase-remove idiom to delete elements matching the selector
                            childrenToProcess.erase(
                                std::remove_if(childrenToProcess.begin(), childrenToProcess.end(),
                                    [&](const std::shared_ptr<BaseNode>& child) {
                                        if (child->getType() == NodeType::Element) {
                                            auto elem = std::dynamic_pointer_cast<ElementNode>(child);
                                            // Simple tag name selector for now
                                            return elem->tagName == deleteInstruction->targetSelector;
                                        }
                                        return false;
                                    }),
                                childrenToProcess.end());
                        }
                    }
                }

                // Finally, resolve the (potentially modified) children
                return resolveChildren(childrenToProcess);
            }
            return {}; // Other template types are not nodes
        }
        case NodeType::Text: {
            return { cloneNode<TextNode>(node) };
        }
        case NodeType::Comment: {
             return { cloneNode<CommentNode>(node) };
        }
        case NodeType::Origin: {
            return { cloneNode<OriginNode>(node) };
        }
        default:
            return {}; // Return empty vector for unknown or irrelevant nodes
    }
}

std::shared_ptr<ElementNode> CHTLResolver::resolveElement(const std::shared_ptr<ElementNode>& node) {
    // Create a new element, cloning properties
    auto newElement = std::make_shared<ElementNode>(node->tagName);
    newElement->attributes = node->attributes;
    newElement->inlineStyles = node->inlineStyles; // Copy existing styles first

    // --- Resolve @Style usages ---
    for (const auto& usage : node->styleUsages) {
        if (context.styleTemplates.count(usage->name)) {
            // It's a template
            auto templateDef = context.styleTemplates[usage->name];
            for (const auto& pair : templateDef->styleProperties) {
                // Add or overwrite properties from the template
                newElement->inlineStyles[pair.first] = pair.second;
            }
        } else if (context.styleCustoms.count(usage->name)) {
            // It's a custom style
            auto customDef = context.styleCustoms[usage->name];
            for (const auto& pair : customDef->styleProperties) {
                // Add or overwrite properties from the custom definition
                newElement->inlineStyles[pair.first] = pair.second;
            }
            // Note: valueless properties and specialization are not handled yet.
        } else {
            std::cerr << "Warning: Undefined style template or custom: " << usage->name << std::endl;
        }

        // After merging, apply deletions
        for (const auto& propToDelete : usage->deletedProperties) {
            newElement->inlineStyles.erase(propToDelete);
        }
    }

    // Resolve the children
    newElement->children = resolveChildren(node->children);

    return newElement;
}

std::vector<std::shared_ptr<BaseNode>> CHTLResolver::resolveChildren(const std::vector<std::shared_ptr<BaseNode>>& children) {
    std::vector<std::shared_ptr<BaseNode>> resolvedChildren;
    for (const auto& child : children) {
        auto resolvedNodes = resolveNode(child);
        // Insert all the resolved nodes (could be 1, or many if a template was expanded)
        resolvedChildren.insert(resolvedChildren.end(), resolvedNodes.begin(), resolvedNodes.end());
    }
    return resolvedChildren;
}
