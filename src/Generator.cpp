#include "Generator.h"
#include <iostream>

std::string Generator::get_html() {
    return html_stream.str();
}

std::string Generator::get_css() {
    return css_stream.str();
}

void Generator::generate(BaseNode& root) {
    root.accept(*this);
}

void Generator::visit(ElementNode& node) {
    // Store the current element to give context to child nodes (like StyleNode)
    ElementNode* previous_element = current_element;
    current_element = &node;

    html_stream << "<" << node.tag_name;

    // This is a simplification. A better approach would be to collect all styles
    // from StyleNodes first, then render attributes.
    std::string inline_style;

    // Visit style nodes first to extract inline styles
    for (const auto& child : node.children) {
        if (auto* style_node = dynamic_cast<StyleNode*>(child.get())) {
            // Simple parsing for "key: value;"
            // This is a placeholder for a real CSS parser.
            inline_style += style_node->raw_css;
        }
    }

    // Render attributes
    for (const auto& attr : node.attributes) {
        attr->accept(*this);
    }

    // Add collected inline styles
    if (!inline_style.empty()) {
        html_stream << " style=\"" << inline_style << "\"";
    }

    html_stream << ">";

    // Visit non-style children to render content
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
             child->accept(*this);
        }
    }

    html_stream << "</" << node.tag_name << ">";

    // Restore previous element context
    current_element = previous_element;
}

void Generator::visit(TextNode& node) {
    html_stream << node.content;
}

void Generator::visit(AttributeNode& node) {
    html_stream << " " << node.key << "=\"" << node.value << "\"";
}

void Generator::visit(StyleNode& node) {
    // The logic is handled in visit(ElementNode&) for now.
    // A more advanced generator would parse the CSS here and add rules
    // to the main css_stream if they are not inline styles.
    // For example, handling .class selectors.
}

void Generator_visit_CommentNode(CommentNode& node) {
    // The spec says # comments are for the generator.
    // We will translate them to HTML comments.
    // html_stream << "<!--" << node.content << " -->";
}
void Generator::visit(CommentNode& node) {
    // The spec says # comments are for the generator.
    // We will translate them to HTML comments.
    html_stream << "<!--" << node.content << " -->";
}


void Generator::indent(int level) {
    for (int i = 0; i < level; ++i) {
        html_stream << "  ";
    }
}
