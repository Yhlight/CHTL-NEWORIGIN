#include "CHTLGenerator.h"
// FIX: Include the full definitions of the node types needed for implementation.
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/AttributeNode.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLGenerator::generate(const BaseNode& root) {
    _output.str("");
    _indent_level = 0;
    generateNode(root);
    return _output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < _indent_level; ++i) {
        _output << "  ";
    }
}

void CHTLGenerator::generateNode(const BaseNode& node) {
    if (const auto* element = dynamic_cast<const ElementNode*>(&node)) {
        generateElement(*element);
    } else if (const auto* text = dynamic_cast<const TextNode*>(&node)) {
        generateText(*text);
    }
}

void CHTLGenerator::generateElement(const ElementNode& node) {
    indent();
    _output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        _output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    _output << ">\n";

    _indent_level++;
    for (const auto& child : node.children) {
        generateNode(*child);
    }
    _indent_level--;

    indent();
    _output << "</" << node.tagName << ">\n";
}

void CHTLGenerator::generateText(const TextNode& node) {
    indent();
    _output << node.content << "\n";
}

} // namespace CHTL
