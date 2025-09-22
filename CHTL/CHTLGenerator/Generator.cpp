#include "Generator.h"

// --- TemplateCollector Implementation ---

void TemplateCollector::visit(ElementNode& node) {
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void TemplateCollector::visit(StyleBlockNode& node) {
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void TemplateCollector::visit(TemplateDefinitionNode& node) {
    templates[node.name] = std::make_shared<TemplateDefinitionNode>(node);
}

// --- Generator Implementation ---

std::string Generator::generate(const std::vector<std::shared_ptr<BaseNode>>& nodes) {
    // Pass 1: Collect all template definitions
    TemplateCollector collector;
    for (const auto& node : nodes) {
        node->accept(collector);
    }
    this->templates = collector.templates;

    // Pass 2: Generate the output
    for (const auto& node : nodes) {
        // Don't generate top-level template definitions directly
        if (!std::dynamic_pointer_cast<TemplateDefinitionNode>(node)) {
            node->accept(*this);
        }
    }
    return output_stream.str();
}

void Generator::visit(ElementNode& node) {
    do_indent();
    output_stream << "<" << node.tag_name;

    // --- Collect and generate style attribute ---
    std::stringstream style_string_stream;
    for (const auto& child : node.children) {
        if (auto style_block = std::dynamic_pointer_cast<StyleBlockNode>(child)) {
            for (const auto& property_node_base : style_block->children) {
                // Handle direct properties
                if (auto property_node = std::dynamic_pointer_cast<StylePropertyNode>(property_node_base)) {
                    style_string_stream << property_node->name << ":" << property_node->value << ";";
                }
                // Handle @Style template usage
                else if (auto usage_node = std::dynamic_pointer_cast<TemplateUsageNode>(property_node_base)) {
                    if (usage_node->type == TemplateType::STYLE) {
                        auto template_def = templates.find(usage_node->name);
                        if (template_def != templates.end()) {
                            // Recursively apply styles from the template
                            for (const auto& template_child : template_def->second->children) {
                                if (auto template_prop = std::dynamic_pointer_cast<StylePropertyNode>(template_child)) {
                                     style_string_stream << template_prop->name << ":" << template_prop->value << ";";
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (const auto& attr : node.attributes) {
        output_stream << " " << attr.first << "=\"" << attr.second << "\"";
    }

    std::string style_attr = style_string_stream.str();
    if (!style_attr.empty()) {
        output_stream << " style=\"" << style_attr << "\"";
    }

    output_stream << ">" << std::endl;
    indent_level++;

    // --- Render children ---
    for (const auto& child : node.children) {
        if (!std::dynamic_pointer_cast<StyleBlockNode>(child)) {
            child->accept(*this);
        }
    }

    indent_level--;
    do_indent();
    output_stream << "</" << node.tag_name << ">" << std::endl;
}

void Generator::visit(TextNode& node) {
    do_indent();
    output_stream << node.text_content << std::endl;
}

void Generator::visit(TemplateDefinitionNode& node) {
    // This is visited during collection pass, does nothing during generation pass.
}

void Generator::visit(TemplateUsageNode& node) {
    if (node.type == TemplateType::ELEMENT) {
        auto template_def = templates.find(node.name);
        if (template_def != templates.end()) {
            for (const auto& child : template_def->second->children) {
                child->accept(*this);
            }
        }
    }
    // @Style usage is handled inside visit(ElementNode&)
}

// These visitors do nothing because the style information is handled
// by the parent ElementNode's visit method.
void Generator::visit(StyleBlockNode& node) {}
void Generator::visit(StylePropertyNode& node) {}

void Generator::do_indent() {
    for (int i = 0; i < indent_level; ++i) {
        output_stream << "  ";
    }
}
