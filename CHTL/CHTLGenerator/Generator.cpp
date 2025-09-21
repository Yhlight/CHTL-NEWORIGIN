#include "Generator.h"
#include <iostream>

namespace CHTL {

Generator::Generator(const std::vector<NodePtr>& nodes) : rootNodes(nodes) {}

std::string Generator::generate() {
    for (const auto& node : rootNodes) {
        collectCssRules(node);
    }

    for (const auto& node : rootNodes) {
        visit(node);
    }
    return output.str();
}

void Generator::collectCssRules(const NodePtr& node) {
    if (!node) return;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        if (element->styleBlock) {
            for (const auto& rule : element->styleBlock->rules) {
                globalCssRules.push_back(rule);
            }
        }
        for (const auto& child : element->children) {
            collectCssRules(child);
        }
    }
}

void Generator::generateCss() {
    if (globalCssRules.empty()) return;

    indent();
    output << "<style>\n";
    indentLevel++;

    for (const auto& rule : globalCssRules) {
        indent();
        output << rule->selector << " {\n";
        indentLevel++;
        for (const auto& prop : rule->properties) {
            indent();
            output << prop.name << ": " << prop.value << ";\n";
        }
        indentLevel--;
        indent();
        output << "}\n";
    }

    indentLevel--;
    indent();
    output << "</style>\n";
}


void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}

void Generator::visit(const NodePtr& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(text);
    }
    // Add cases for other node types here later
}

void Generator::visitElementNode(const std::shared_ptr<ElementNode>& node) {
    indent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr.name << "=\"" << attr.value << "\"";
    }

    if (node->styleBlock && !node->styleBlock->inlineProperties.empty()) {
        output << " style=\"";
        for (const auto& prop : node->styleBlock->inlineProperties) {
            output << prop.name << ": " << prop.value << ";";
        }
        output << "\"";
    }

    if (node->children.empty()) {
        // Self-closing tag for simplicity, though not all tags are self-closing in HTML
        // A proper implementation would have a list of void elements (area, base, br, etc.)
        output << " />\n";
    } else {
        output << ">\n";
        indentLevel++;

        for (const auto& child : node->children) {
            visit(child);
        }

        if (node->tagName == "head") {
            generateCss();
        }

        indentLevel--;
        indent();
        output << "</" << node->tagName << ">\n";
    }
}

void Generator::visitTextNode(const std::shared_ptr<TextNode>& node) {
    indent();
    output << node->text << "\n";
}

} // namespace CHTL
