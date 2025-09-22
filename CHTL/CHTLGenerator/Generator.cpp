#include "Generator.h"
#include <stdexcept>
#include <iostream>
#include <vector>

Generator::Generator() {}

std::string Generator::generate(const std::vector<std::unique_ptr<BaseNode>>& ast) {
    output.str("");
    output.clear();
    indent_level = 0;

    for (const auto& node : ast) {
        generate_node(node.get());
    }
    return output.str();
}

void Generator::write_indent() {
    output << std::string(indent_level * 2, ' ');
}

void Generator::generate_node(const BaseNode* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::ELEMENT:
            generate_element(static_cast<const ElementNode*>(node));
            break;
        case NodeType::TEXT:
            generate_text(static_cast<const TextNode*>(node));
            break;
        case NodeType::COMMENT:
            generate_comment(static_cast<const CommentNode*>(node));
            break;
        case NodeType::STYLE:
            // Style nodes are handled within generate_element, so we can ignore them here.
            break;
        default:
            break;
    }
}

void Generator::generate_element(const ElementNode* node) {
    write_indent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // Find and process style nodes to generate inline styles
    std::string inline_style;
    for (const auto& child : node->children) {
        if (child->getType() == NodeType::STYLE) {
            const auto* style_node = static_cast<const StyleNode*>(child.get());
            for (const auto& prop : style_node->properties) {
                if (prop.value) {
                    inline_style += prop.key + ": " + *prop.value + "; ";
                }
            }
        }
    }
    if (!inline_style.empty()) {
        // Remove trailing space
        inline_style.pop_back();
        output << " style=\"" << inline_style << "\"";
    }

    // Check if there are any non-style children
    bool has_renderable_children = false;
    for (const auto& child : node->children) {
        if (child->getType() != NodeType::STYLE) {
            has_renderable_children = true;
            break;
        }
    }

    if (!has_renderable_children) {
        output << " />\n";
    } else {
        output << ">\n";
        indent_level++;
        for (const auto& child : node->children) {
            // We only generate non-style nodes, as styles have been processed
            if (child->getType() != NodeType::STYLE) {
                generate_node(child.get());
            }
        }
        indent_level--;
        write_indent();
        output << "</" << node->tagName << ">\n";
    }
}

void Generator::generate_text(const TextNode* node) {
    write_indent();
    output << node->content << "\n";
}

void Generator::generate_comment(const CommentNode* node) {
    write_indent();
    output << "<!-- " << node->content << " -->\n";
}
