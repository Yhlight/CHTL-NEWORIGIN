#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ConfigurationNode.h"
#include "../CHTLNode/PropertyNode.h"
#include <stdexcept>

namespace CHTL {

void CHTLGenerator::generate(const std::shared_ptr<BaseNode>& node) {
    if (node) {
        visit(node);
    }
}

void CHTLGenerator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::NODE_ELEMENT:
            visit(std::dynamic_pointer_cast<ElementNode>(node));
            break;
        case NodeType::NODE_TEXT:
            visit(std::dynamic_pointer_cast<TextNode>(node));
            break;
        case NodeType::NODE_STYLE:
            visit(std::dynamic_pointer_cast<StyleNode>(node));
            break;
        case NodeType::NODE_TEMPLATE:
            visit(std::dynamic_pointer_cast<TemplateNode>(node));
            break;
        case NodeType::NODE_CUSTOM:
            visit(std::dynamic_pointer_cast<CustomNode>(node));
            break;
        case NodeType::NODE_ORIGIN:
            visit(std::dynamic_pointer_cast<OriginNode>(node));
            break;
        case NodeType::NODE_IMPORT:
            visit(std::dynamic_pointer_cast<ImportNode>(node));
            break;
        case NodeType::NODE_NAMESPACE:
            visit(std::dynamic_pointer_cast<NamespaceNode>(node));
            break;
        case NodeType::NODE_CONFIG:
            visit(std::dynamic_pointer_cast<ConfigurationNode>(node));
            break;
        case NodeType::NODE_PROPERTY:
             visit(std::dynamic_pointer_cast<PropertyNode>(node));
            break;
        default:
            // Silently ignore other node types for now
            break;
    }
}

void CHTLGenerator::visit(const std::shared_ptr<ElementNode>& node) {
    html_out << "<" << node->getTagName();

    // Handle explicit attributes
    for (const auto& attr : node->getAttributes()) {
        html_out << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Collect inline styles from child StyleNodes
    std::stringstream style_ss;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::NODE_STYLE) {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(child);
            for (const auto& prop : styleNode->getProperties()) {
                if (prop) {
                    style_ss << prop->getKey() << ": " << prop->getValue() << ";";
                }
            }
        }
    }

    // Add the style attribute if there are any styles
    if (style_ss.rdbuf()->in_avail() > 0) {
        html_out << " style=\"" << style_ss.str() << "\"";
    }

    html_out << ">";

    // Visit non-style children
    for (const auto& child : node->getChildren()) {
        if (child->getType() != NodeType::NODE_STYLE) {
            visit(child);
        }
    }

    html_out << "</" << node->getTagName() << ">";
}

void CHTLGenerator::visit(const std::shared_ptr<TextNode>& node) {
    html_out << node->getContent();
}

void CHTLGenerator::visit(const std::shared_ptr<StyleNode>& node) {
    // Handled by the parent ElementNode to generate an inline style attribute.
    // This method does nothing on its own.
}

void CHTLGenerator::visit(const std::shared_ptr<TemplateNode>& node) {
    // Templates are handled by the compiler and do not generate output directly.
}

void CHTLGenerator::visit(const std::shared_ptr<CustomNode>& node) {
    // Custom blocks are handled by the compiler and do not generate output directly.
}

void CHTLGenerator::visit(const std::shared_ptr<OriginNode>& node) {
    // This will be implemented to handle raw output.
}

void CHTLGenerator::visit(const std::shared_ptr<ImportNode>& node) {
    // Import nodes are handled by a loader, not the generator.
}

void CHTLGenerator::visit(const std::shared_ptr<NamespaceNode>& node) {
    // Namespaces affect scope, but the block itself doesn't generate output.
    // We will visit its children.
    for (const auto& child : node->getChildren()) {
        visit(child);
    }
}

void CHTLGenerator::visit(const std::shared_ptr<ConfigurationNode>& node) {
    // Configuration nodes affect the compiler and do not generate output.
}

void CHTLGenerator::visit(const std::shared_ptr<PropertyNode>& node) {
    // Property nodes are part of other nodes and are handled within their parent's visit method.
}

}