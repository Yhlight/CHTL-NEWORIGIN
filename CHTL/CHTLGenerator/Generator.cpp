#include "Generator.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>

Generator::Generator(const ParsingContext& context) : context(context) {}

GeneratedOutput Generator::generate(const std::vector<std::unique_ptr<BaseNode>>& ast) {
    html_output.str("");
    html_output.clear();
    css_output.str("");
    css_output.clear();
    indent_level = 0;

    for (const auto& node : ast) {
        generate_node(node.get());
    }
    return {html_output.str(), css_output.str()};
}

void Generator::write_indent() {
    html_output << std::string(indent_level * 2, ' ');
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
        case NodeType::TEMPLATE_USAGE: {
            const auto* usage_node = static_cast<const TemplateUsageNode*>(node);
            auto it = context.template_definitions.find(usage_node->name);
            if (it != context.template_definitions.end()) {
                const auto& template_def = it->second;
                if (template_def->template_type == TemplateType::ELEMENT) {
                    const auto& body = std::get<TemplateDefinitionNode::ElementBody>(template_def->body);
                    for (const auto& body_node : body) {
                        generate_node(body_node.get());
                    }
                }
                // Silently ignore usage of non-element templates at the top level
            } else {
                // Or throw an error for undefined templates
            }
            break;
        }
        case NodeType::STYLE:
        case NodeType::TEMPLATE_DEFINITION:
            break;
        default:
            break;
    }
}

void Generator::generate_element(const ElementNode* node) {
    write_indent();
    html_output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        html_output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    std::string inline_style_str;
    std::string class_attr;
    std::string id_attr;

    for (const auto& child : node->children) {
        if (child->getType() == NodeType::STYLE) {
            const auto* style_node = static_cast<const StyleNode*>(child.get());
            for (const auto& prop : style_node->inline_properties) {
                if (prop.value) {
                    inline_style_str += prop.key + ": " + *prop.value + "; ";
                }
            }
            for (const auto& rule : style_node->style_rules) {
                if (!rule.selector.empty()) {
                    if (rule.selector[0] == '.') {
                        if (!class_attr.empty()) class_attr += " ";
                        class_attr += rule.selector.substr(1);
                    } else if (rule.selector[0] == '#') {
                        if (!id_attr.empty()) id_attr += " ";
                        id_attr += rule.selector.substr(1);
                    }
                }
                if(!rule.properties.empty()){
                    css_output << rule.selector << " {\n";
                    for (const auto& prop : rule.properties) {
                        if(prop.value) {
                            css_output << "  " << prop.key << ": " << *prop.value << ";\n";
                        }
                    }
                    css_output << "}\n";
                }
            }
        }
    }

    if (!class_attr.empty()) html_output << " class=\"" << class_attr << "\"";
    if (!id_attr.empty()) html_output << " id=\"" << id_attr << "\"";
    if (!inline_style_str.empty()) {
        inline_style_str.pop_back();
        html_output << " style=\"" << inline_style_str << "\"";
    }

    bool has_renderable_children = false;
    for (const auto& child : node->children) {
        if (child->getType() != NodeType::STYLE) {
            has_renderable_children = true;
            break;
        }
    }

    if (!has_renderable_children) {
        html_output << " />\n";
    } else {
        html_output << ">\n";
        indent_level++;
        for (const auto& child : node->children) {
            if (child->getType() != NodeType::STYLE) {
                generate_node(child.get());
            }
        }
        indent_level--;
        write_indent();
        html_output << "</" << node->tagName << ">\n";
    }
}

void Generator::generate_text(const TextNode* node) {
    write_indent();
    html_output << node->content << "\n";
}

void Generator::generate_comment(const CommentNode* node) {
    write_indent();
    html_output << "<!-- " << node->content << " -->\n";
}
