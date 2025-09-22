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
    this->ast_root = &root;
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
    std::string previous_namespace = current_namespace;
    current_namespace = node.name;
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    current_namespace = previous_namespace;
}

void Generator::visit(ConfigurationNode& node) {}
void Generator::visit(DeleteNode& node) {}
void Generator::visit(UseNode& node) {
    if (node.value == "html5") {
        use_html5 = true;
    }
}
void Generator::visit(ImportNode& node) {}

void Generator::visit(ElementNode& node) {
    ElementNode* previous_element = current_element;
    current_element = &node;

    html_stream << "<" << node.tag_name;

    // --- Style pre-pass ---
    std::vector<AttributeNode*> style_attributes;

    for (const auto& child : node.children) {
        if (auto* style_usage = dynamic_cast<StyleUsageNode*>(child.get())) {
            std::string ns = style_usage->from_namespace.value_or(current_namespace);
            if (style_templates[ns].count(style_usage->name)) {
                auto* template_node = style_templates[ns][style_usage->name];
                collect_styles_recursive(template_node, style_attributes);

                if (template_node->is_custom && !style_usage->specializations.empty()) {
                    for (const auto& spec_node : style_usage->specializations) {
                        if (auto* spec_attr = dynamic_cast<AttributeNode*>(spec_node.get())) {
                            style_attributes.push_back(spec_attr);
                        }
                    }
                }
            }
        }
    }

    std::string inline_styles_str;
    for (const auto& child : node.children) {
        if (auto* style_node = dynamic_cast<StyleNode*>(child.get())) {
            inline_styles_str += style_node->raw_css;
        }
    }

    for (const auto& attr : node.attributes) {
        attr->accept(*this);
    }

    if (!style_attributes.empty() || !inline_styles_str.empty()) {
        html_stream << " style=\"";

        std::map<std::string, std::string> final_styles;
        for (const auto* attr : style_attributes) {
            std::string value_str;
            if (auto val_ptr = std::get_if<std::optional<std::string>>(&attr->value)) {
                if (val_ptr->has_value()) value_str = **val_ptr;
            } else if (auto var_ptr = std::get_if<std::unique_ptr<VarUsageNode>>(&attr->value)) {
                value_str = evaluate_var_usage(*var_ptr->get());
            } else if (auto expr_ptr = std::get_if<std::unique_ptr<ExprNode>>(&attr->value)) {
                value_str = evaluate_expression(*expr_ptr->get());
            }
            if (!value_str.empty()) final_styles[attr->key] = value_str;
        }

        for (const auto& child : node.children) {
            if (auto* style_usage = dynamic_cast<StyleUsageNode*>(child.get())) {
                 std::string ns = style_usage->from_namespace.value_or(current_namespace);
                if (style_templates[ns].count(style_usage->name)) {
                    auto* template_node = style_templates[ns][style_usage->name];
                    if (template_node->is_custom) {
                        for (const auto& spec_node : style_usage->specializations) {
                            if (auto* delete_node = dynamic_cast<DeleteNode*>(spec_node.get())) {
                                for (const auto& key : delete_node->keys_to_delete) {
                                    final_styles.erase(key);
                                }
                            }
                        }
                    }
                }
            }
        }

        for(const auto& pair : final_styles) {
            html_stream << pair.first << ": " << pair.second << "; ";
        }
        html_stream << inline_styles_str;
        html_stream << "\"";
    }

    html_stream << ">";

    // --- Content pass ---
    for (const auto& child : node.children) {
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
        if (val_ptr->has_value()) html_stream << **val_ptr;
    } else if (auto var_ptr = std::get_if<std::unique_ptr<VarUsageNode>>(&node.value)) {
        html_stream << evaluate_var_usage(*var_ptr->get());
    } else if (auto expr_ptr = std::get_if<std::unique_ptr<ExprNode>>(&node.value)) {
        html_stream << evaluate_expression(*expr_ptr->get());
    }

    html_stream << "\"";
}

void Generator::visit(StyleNode& node) {}
void Generator::visit(CommentNode& node) {
    html_stream << "<!--" << node.content << " -->";
}

void Generator::visit(TemplateStyleNode& node) {
    style_templates[current_namespace][node.name] = &node;
}

void Generator::visit(TemplateElementNode& node) {
    element_templates[current_namespace][node.name] = &node;
}

void Generator::visit(TemplateVarNode& node) {
    var_templates[current_namespace][node.name] = &node;
}

void Generator::visit(VarUsageNode& node) {
    html_stream << evaluate_var_usage(node);
}

void Generator::visit(StyleUsageNode& node) {}

void Generator::visit(ElementUsageNode& node) {
    std::string ns = node.from_namespace.value_or(current_namespace);
    if (element_templates[ns].count(node.name)) {
        auto* template_node = element_templates[ns][node.name];
        for (const auto& child : template_node->children) {
            child->accept(*this);
        }
    }
}

void Generator::collect_styles_recursive(TemplateStyleNode* node, std::vector<AttributeNode*>& collected_styles) {
    if (!node) return;

    for (const auto& child : node->children) {
        if (auto* usage = dynamic_cast<StyleUsageNode*>(child.get())) {
            std::string ns = usage->from_namespace.value_or(current_namespace);
            if (style_templates[ns].count(usage->name)) {
                collect_styles_recursive(style_templates[ns][usage->name], collected_styles);
            }
        }
    }

    for (const auto& child : node->children) {
        if (auto* attr = dynamic_cast<AttributeNode*>(child.get())) {
            collected_styles.push_back(attr);
        }
    }
}

std::string Generator::evaluate_var_usage(VarUsageNode& node) {
    std::string ns = current_namespace; // Simplified: doesn't handle 'from' on var usage yet
    if (var_templates[ns].count(node.group_name)) {
        auto* template_node = var_templates[ns][node.group_name];
        for (const auto& var_attr : template_node->variables) {
            if (var_attr->key == node.var_name) {
                if (auto val_ptr = std::get_if<std::optional<std::string>>(&var_attr->value)) {
                    if (val_ptr->has_value()) return **val_ptr;
                }
            }
        }
    }
    return "[VAR NOT FOUND]";
}

std::string Generator::evaluate_expression(ExprNode& node) {
    if (auto* n = dynamic_cast<NumberNode*>(&node)) {
        std::ostringstream oss;
        oss << n->value;
        return oss.str() + n->unit;
    }
    if (auto* b = dynamic_cast<BinaryOpNode*>(&node)) {
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
    if (auto* p = dynamic_cast<PropertyReferenceNode*>(&node)) {
        if (!ast_root) return "[REF ERROR: NO ROOT]";
        auto* target_node = find_node_by_selector(ast_root, p->selector);
        if (!target_node) return "[REF ERROR: NOT FOUND]";

        for (const auto& attr : target_node->attributes) {
            if (attr->key == p->property_name) {
                if (auto val_ptr = std::get_if<std::optional<std::string>>(&attr->value)) {
                    if (val_ptr->has_value()) return **val_ptr;
                }
            }
        }

        return "[REF ERROR: PROP NOT FOUND]";
    }
    return "[EXPR ERROR]";
}

ElementNode* Generator::find_node_by_selector(BaseNode* start_node, const std::string& selector) {
    if (!start_node) return nullptr;

    if (auto* element = dynamic_cast<ElementNode*>(start_node)) {
        for (const auto& attr : element->attributes) {
            if (attr->key == "id") {
                if (auto val_ptr = std::get_if<std::optional<std::string>>(&attr->value)) {
                     if ((*val_ptr).has_value() && ("#" + **val_ptr) == selector) {
                        return element;
                    }
                }
            }
        }
    }

    if (auto* p_node = dynamic_cast<ProgramNode*>(start_node)) {
        for (const auto& child : p_node->children) {
            if (auto* found = find_node_by_selector(child.get(), selector)) return found;
        }
    } else if (auto* e_node = dynamic_cast<ElementNode*>(start_node)) {
        for (const auto& child : e_node->children) {
            if (auto* found = find_node_by_selector(child.get(), selector)) return found;
        }
    } else if (auto* ns_node = dynamic_cast<NamespaceNode*>(start_node)) {
         for (const auto& child : ns_node->children) {
            if (auto* found = find_node_by_selector(child.get(), selector)) return found;
        }
    }

    return nullptr;
}


void Generator::indent(int level) {
    for (int i = 0; i < level; ++i) {
        html_stream << "  ";
    }
}
