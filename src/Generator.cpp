#include "Generator.h"
#include <iostream>
#include <vector>
#include <sstream>

std::string Generator::get_html() {
    if (use_html5) {
        return "<!DOCTYPE html>\n" + html_stream.str();
    }
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

void Generator::visit(UseNode& node) {
    if (node.value == "html5") {
        use_html5 = true;
    }
}

void Generator::visit(ImportNode& node) {
    // Imports are handled by the Loader, so the generator does nothing.
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

                // If it's a custom style with specializations, we need to merge
                if (template_node->is_custom && !style_usage->specializations.empty()) {
                    for(const auto& base_style : template_node->styles) {
                        bool has_value = true;
                        if (auto val_ptr = std::get_if<std::optional<std::string>>(&base_style->value)) {
                            if (!(*val_ptr).has_value()) {
                                has_value = false;
                            }
                        }

                        if (!has_value) { // Valueless property like 'color;'
                            // Find the specialization for this key
                            for (const auto& spec_style : style_usage->specializations) {
                                if (spec_style->key == base_style->key) {
                                    style_attributes.push_back(spec_style.get());
                                    break;
                                }
                            }
                        } else { // Property with a default value
                            style_attributes.push_back(base_style.get());
                        }
                    }
                } else { // Regular template usage
                    for (const auto& style_attr : template_node->styles) {
                        style_attributes.push_back(style_attr.get());
                    }
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
            html_stream << attr->key << ": ";
            if (auto val_ptr = std::get_if<std::optional<std::string>>(&attr->value)) {
                if (val_ptr->has_value()) {
                    html_stream << **val_ptr;
                }
            } else if (auto var_ptr = std::get_if<std::unique_ptr<VarUsageNode>>(&attr->value)) {
                html_stream << evaluate_var_usage(*var_ptr->get());
            } else if (auto expr_ptr = std::get_if<std::unique_ptr<ExprNode>>(&attr->value)) {
                html_stream << evaluate_expression(*expr_ptr->get());
            }
            html_stream << "; ";
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
    html_stream << " " << node.key << "=\"";

    if (auto val_ptr = std::get_if<std::optional<std::string>>(&node.value)) {
        if (val_ptr->has_value()) {
            html_stream << **val_ptr;
        }
    } else if (auto var_ptr = std::get_if<std::unique_ptr<VarUsageNode>>(&node.value)) {
        html_stream << evaluate_var_usage(*var_ptr->get());
    } else if (auto expr_ptr = std::get_if<std::unique_ptr<ExprNode>>(&node.value)) {
        html_stream << evaluate_expression(*expr_ptr->get());
    }

    html_stream << "\"";
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
    var_templates[node.name] = &node;
}

void Generator::visit(VarUsageNode& node) {
    // This node doesn't generate output directly.
    // It is evaluated when found inside an AttributeNode.
    html_stream << evaluate_var_usage(node);
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

std::string Generator::evaluate_var_usage(VarUsageNode& node) {
    if (var_templates.count(node.group_name)) {
        auto* template_node = var_templates[node.group_name];
        for (const auto& var_attr : template_node->variables) {
            if (var_attr->key == node.var_name) {
                // Assuming the value is a simple string for now
                if (auto val_ptr = std::get_if<std::optional<std::string>>(&var_attr->value)) {
                    if (val_ptr->has_value()) {
                        return **val_ptr;
                    }
                }
            }
        }
    }
    return "[VAR NOT FOUND]"; // Error case
}

std::string Generator::evaluate_expression(ExprNode& node) {
    if (auto* n = dynamic_cast<NumberNode*>(&node)) {
        std::ostringstream oss;
        oss << n->value;
        return oss.str() + n->unit;
    }
    if (auto* b = dynamic_cast<BinaryOpNode*>(&node)) {
        // This is a very simplified evaluation. It doesn't check for compatible units.
        double left_val = std::stod(evaluate_expression(*b->left));
        double right_val = std::stod(evaluate_expression(*b->right));
        double result = 0;
        switch (b->op) {
            case '+': result = left_val + right_val; break;
            case '-': result = left_val - right_val; break;
            case '*': result = left_val * right_val; break;
            case '/': result = left_val / right_val; break;
        }

        std::string unit;
        if(auto* n_left = dynamic_cast<NumberNode*>(b->left.get())) {
            unit = n_left->unit;
        }
        std::ostringstream oss;
        oss << result;
        return oss.str() + unit;
    }
    return "[EXPR ERROR]";
}


void Generator::indent(int level) {
    for (int i = 0; i < level; ++i) {
        html_stream << "  ";
    }
}
