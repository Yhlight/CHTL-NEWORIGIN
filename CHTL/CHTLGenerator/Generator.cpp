#include "Generator.h"
#include <stdexcept>

namespace CHTL {

Generator::Generator() {}

std::string Generator::generate(const ElementNode& root) {
    output.str(""); // Clear the stream
    output.clear();
    indentLevel = 0;

    // We don't render the virtual "root" element itself, just its children.
    for (const auto& child : root.children) {
        visit(*child);
    }

    return output.str();
}

void Generator::visit(const BaseNode& node) {
    if (const auto* elementNode = dynamic_cast<const ElementNode*>(&node)) {
        visitElement(*elementNode);
    } else if (const auto* textNode = dynamic_cast<const TextNode*>(&node)) {
        visitText(*textNode);
    } else if (const auto* commentNode = dynamic_cast<const CommentNode*>(&node)) {
        visitComment(*commentNode);
    } else {
        // In a real compiler, you might want more robust error handling.
        // For now, we'll just ignore unknown node types.
    }
}

void Generator::visitElement(const ElementNode& node) {
    indent();
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }
    output << ">" << std::endl;

    indentLevel++;
    for (const auto& child : node.children) {
        visit(*child);
    }
    indentLevel--;

    indent();
    output << "</" << node.tagName << ">" << std::endl;
}

void Generator::visitText(const TextNode& node) {
    indent();
    output << node.content << std::endl;
}

void Generator::visitComment(const CommentNode& node) {
    // Per the spec, only generator comments (#) are rendered.
    if (node.type == CommentType::GENERATOR) {
        indent();
        // The content includes the '#', so we skip it.
        // Also trim leading space if present.
        size_t start = 1;
        if (node.content.length() > 1 && node.content[1] == ' ') {
            start = 2;
        }
        output << "<!-- " << node.content.substr(start) << " -->" << std::endl;
    }
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}

} // namespace CHTL
