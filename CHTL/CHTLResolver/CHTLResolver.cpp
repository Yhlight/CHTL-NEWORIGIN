#include "CHTLResolver.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include <iostream>

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
                if (context.elementTemplates.count(usageNode->name)) {
                    auto templateDef = context.elementTemplates[usageNode->name];
                    // Recursively resolve the children of the template definition
                    return resolveChildren(templateDef->children);
                } else {
                    // In a real compiler, we'd throw an error here. For now, just ignore.
                    std::cerr << "Warning: Undefined element template used: " << usageNode->name << std::endl;
                    return {};
                }
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
    newElement->inlineStyles = node->inlineStyles;

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
