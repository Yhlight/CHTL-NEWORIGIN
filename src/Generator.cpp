#include "Generator.h"
#include <iostream>
#include <vector>

std::string Generator::get_html() {
    return html_stream.str();
}

std::string Generator::get_css() {
    return css_stream.str();
}

void Generator::generate(ProgramNode& root) {
    // First pass: register all template definitions
    root.accept(*this);
    // Second pass: generate the output
    html_stream.str(""); // Clear the stream
    root.accept(*this);
}

void Generator::visit(ProgramNode& node) {
    for (const auto& child : node.children) {
        if (child) {
            child->accept(*this);
        }
    }
}

void Generator::visit(NamespaceNode& node) {
    // For now, we just generate the content of the namespace directly.
    // Scoping will be handled later.
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void Generator::visit(ConfigurationNode& node) {
    // Configuration nodes do not generate output.
    // They are used to configure the compiler itself.
}

void Generator::visit(ElementNode& node) {
    ElementNode* previous_element = current_element;
    current_element = &node;

    html_stream << "<" << node.tag_name;

    // --- Style pre-pass ---
    std::vector<AttributeNode*> style_attributes;

    // Collect styles from @Style templates
    for (const auto& child : node.children) {
        if (auto* style_usage = dynamic_cast<StyleUsageNode*>(child.get())) {
            if (style_templates.count(style_usage->name)) {
                auto* template_node = style_templates[style_usage->name];
                for (const auto& style_attr : template_node->styles) {
                    style_attributes.push_back(style_attr.get());
                }
            }
        }
    }

    // Collect styles from inline style blocks
    for (const auto& child : node.children) {
        if (auto* style_node = dynamic_cast<StyleNode*>(child.get())) {
            // This is a simplification. A real implementation would parse the raw_css.
            // For now, we assume raw_css is empty and we're not supporting inline style blocks
            // when also using style templates, to keep this step manageable.
        }
    }

    // Render element's own attributes
    for (const auto& attr : node.attributes) {
        attr->accept(*this);
    }

    // Render collected styles into a style="..." attribute
    if (!style_attributes.empty()) {
        html_stream << " style=\"";
        for (const auto* attr : style_attributes) {
            if (auto val_ptr = std::get_if<std::optional<std::string>>(&attr->value)) {
                if (val_ptr->has_value()) {
                    html_stream << attr->key << ": " << **val_ptr << "; ";
                }
            }
            // Later, handle ExprNode here by evaluating it
        }
        html_stream << "\"";
    }

    html_stream << ">";

    // --- Content pass ---
    for (const auto& child : node.children) {
        // Skip nodes that are not for content generation
        if (dynamic_cast<StyleNode*>(child.get()) || dynamic_cast<StyleUsageNode*>(child.get())) {
            continue;
        }
        child->accept(*this);
    }

    html_stream << "</" << node.tag_name << ">";

    current_element = previous_element;
}

void Generator::visit(TextNode& node) {
    html_stream << node.content;
}

void Generator::visit(AttributeNode& node) {
    if (auto val_ptr = std::get_if<std::optional<std::string>>(&node.value)) {
        if (val_ptr->has_value()) {
            html_stream << " " << node.key << "=\"" << **val_ptr << "\"";
        }
    }
    // Later, handle ExprNode here
}

void Generator::visit(StyleNode& node) {
    // Logic is handled in ElementNode visit's pre-pass
}

void Generator::visit(CommentNode& node) {
    html_stream << "<!--" << node.content << " -->";
}

void Generator::visit(TemplateStyleNode& node) {
    style_templates[node.name] = &node;
}

void Generator::visit(TemplateElementNode& node) {
    element_templates[node.name] = &node;
}

void Generator::visit(TemplateVarNode& node) {
    // Not implemented yet
}

void Generator::visit(StyleUsageNode& node) {
    // Logic is handled within ElementNode visit's pre-pass
}

void Generator::visit(ElementUsageNode& node) {
    if (element_templates.count(node.name)) {
        auto* template_node = element_templates[node.name];
        for (const auto& child : template_node->children) {
            child->accept(*this);
        }
    }
}


void Generator::indent(int level) {
    for (int i = 0; i < level; ++i) {
        html_stream << "  ";
    }
}
