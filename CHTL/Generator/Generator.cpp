#include "Generator.h"
#include "CHTL/Node/ElementNode.h"
#include "CHTL/Node/TextNode.h"
#include "CHTL/Node/CommentNode.h"
#include <regex>

std::string Generator::generate(const ParseResult& result) {
    output.str(""); // Clear the stream

    // 1. Render global styles
    if (!result.global_styles.empty()) {
        output << "<style>\n";
        for (const auto& rule : result.global_styles) {
            std::string selector = rule.selector;
            // Handle '&' substitution
            if (selector.find('&') != std::string::npos) {
                selector = std::regex_replace(selector, std::regex("&"), rule.context_selector);
            }
            output << "  " << selector << " {\n";
            for (const auto& prop : rule.properties) {
                output << "    " << prop.key << ": " << prop.value << ";\n";
            }
            output << "  }\n";
        }
        output << "</style>\n\n";
    }

    // 2. Render AST
    for (const auto& node : result.ast) {
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
