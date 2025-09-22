#include "Generator.h"
#include "CHTL/Node/ElementNode.h"
#include "CHTL/Node/TextNode.h"
#include "CHTL/Node/CommentNode.h"

std::string Generator::generate(const NodeList& ast) {
    output.str(""); // Clear the stream
    for (const auto& node : ast) {
        if (node) {
            node->accept(*this);
            output << "\n";
        }
    }
    return output.str();
}

void Generator::indent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "  ";
    }
}

void Generator::visit(const ElementNode& node) {
    indent();
    output << "<" << node.tag_name;

    // Add attributes
    for (const auto& attr : node.attributes) {
        output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // Combine style properties into a single style attribute
    if (!node.styles.empty()) {
        output << " style=\"";
        for (size_t i = 0; i < node.styles.size(); ++i) {
            output << node.styles[i].key << ":" << node.styles[i].value << ";";
        }
        output << "\"";
    }

    output << ">";

    // Handle children
    if (!node.children.empty()) {
        output << "\n";
        indent_level++;
        for (const auto& child : node.children) {
            child->accept(*this);
            output << "\n";
        }
        indent_level--;
        indent();
    }

    output << "</" << node.tag_name << ">";
}

void Generator::visit(const TextNode& node) {
    indent();
    output << node.text;
}

void Generator::visit(const CommentNode& node) {
    indent();
    output << "<!-- " << node.comment << " -->";
}
