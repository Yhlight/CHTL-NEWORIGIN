#include "CHTLGenerator.h"
#include <iostream>

CHTLGenerator::CHTLGenerator(std::shared_ptr<ElementNode> root) : rootNode(root) {}

std::string CHTLGenerator::generate() {
    output.str(""); // Clear the stream
    output.clear();
    visit(rootNode);
    return output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}

void CHTLGenerator::visit(const std::shared_ptr<BaseNode>& node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            visitElement(std::dynamic_pointer_cast<ElementNode>(node));
            break;
        case NodeType::Text:
            visitText(std::dynamic_pointer_cast<TextNode>(node));
            break;
        case NodeType::Comment:
            visitComment(std::dynamic_pointer_cast<CommentNode>(node));
            break;
        default:
            // Ignore other node types for now
            break;
    }
}

void CHTLGenerator::visitElement(const std::shared_ptr<ElementNode>& node) {
    if (node->tagName == "__ROOT__") {
        // Special case for the virtual root, just process its children
        for (const auto& child : node->children) {
            visit(child);
        }
        return;
    }

    indent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    // Generate inline styles from the evaluated results
    if (!node->finalStyles.empty()) {
        output << " style=\"";
        for (const auto& stylePair : node->finalStyles) {
            output << stylePair.first << ": " << stylePair.second << ";";
        }
        output << "\"";
    }

    if (selfClosingTags.count(node->tagName)) {
        output << " />\n";
        return;
    }

    output << ">\n";
    indentLevel++;

    for (const auto& child : node->children) {
        visit(child);
    }

    indentLevel--;
    indent();
    output << "</" << node->tagName << ">\n";
}

void CHTLGenerator::visitText(const std::shared_ptr<TextNode>& node) {
    indent();
    output << node->content << "\n";
}

void CHTLGenerator::visitComment(const std::shared_ptr<CommentNode>& node) {
    if (node->isGeneratorAware) {
        indent();
        output << "<!-- " << node->content << " -->\n";
    }
    // Non-generator-aware comments are ignored as per the spec
}
