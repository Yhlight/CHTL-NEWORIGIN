#include "Generator.h"
#include "../Node/StyleNode.h"
#include "../Node/TemplateUsageNode.h"
#include <stdexcept>
#include <algorithm>

namespace CHTL {

Generator::Generator(TemplateStore& store) : store(store) {}

std::string Generator::generate(ElementNode& root) {
    std::string html_output;
    global_css = "";

    for (auto& child : root.children) {
        generateNode(*child, html_output, 0);
    }

    if (!global_css.empty()) {
        std::string style_tag = "<style>\\n" + global_css + "</style>\\n";
        html_output.insert(0, style_tag);
    }

    return html_output;
}

void Generator::generateNode(BaseNode& node, std::string& output, int indent) {
    switch (node.getType()) {
        case NodeType::Element:
            generateElementNode(static_cast<ElementNode&>(node), output, indent);
            break;
        case NodeType::Text:
            generateTextNode(static_cast<const TextNode&>(node), output, indent);
            break;
        case NodeType::TemplateUsage:
            {
                auto& usage_node = static_cast<TemplateUsageNode&>(node);
                if (usage_node.template_type == TemplateType::Element) {
                    const auto* tmpl = store.getElementTemplate(usage_node.name);
                    if (tmpl) {
                        for (auto& n : tmpl->nodes) {
                            generateNode(*n, output, indent);
                        }
                    } else {
                        throw std::runtime_error("Generator Error: Element template '" + usage_node.name + "' not found.");
                    }
                }
                // @Style usage is handled within generateElementNode
            }
            break;
        case NodeType::Style:
            // Style nodes are handled by their parent ElementNode.
            break;
        default:
            break;
    }
}

void Generator::generateElementNode(ElementNode& node, std::string& output, int indent) {
    std::string indent_str(indent * 2, ' ');

    StyleNode* style_node = nullptr;
    for (const auto& child : node.children) {
        if (child->getType() == NodeType::Style) {
            style_node = static_cast<StyleNode*>(child.get());
            break;
        }
    }

    // --- Pre-processing pass for this node to handle style rules ---
    if (style_node) {
        // Apply base style templates from @Style usage
        for (const auto& child : style_node->children) {
            if (child->getType() == NodeType::TemplateUsage) {
                auto& usage_node = static_cast<TemplateUsageNode&>(*child);
                if (usage_node.template_type == TemplateType::Style) {
                    const auto* tmpl = store.getStyleTemplate(usage_node.name);
                    if (tmpl) {
                        for (const auto& prop : tmpl->properties) {
                            // Check if property should be deleted
                            bool deleted = false;
                            for (const auto& deleted_prop : usage_node.deleted_properties) {
                                if (prop.first == deleted_prop) {
                                    deleted = true;
                                    break;
                                }
                            }
                            if (!deleted) {
                                style_node->addInlineProperty(prop.first, prop.second);
                            }
                        }
                    }
                }
            }
        }

        for (const auto& rule : style_node->rules) {
             if (rule.selector[0] == '.') {
                std::string class_name = rule.selector.substr(1);
                bool class_exists = false;
                for(auto& attr : node.attributes) {
                    if (attr.first == "class") {
                        if (attr.second.find(class_name) == std::string::npos) {
                           attr.second += " " + class_name;
                        }
                        class_exists = true;
                        break;
                    }
                }
                if (!class_exists) {
                    node.addAttribute("class", class_name);
                }
            } else if (rule.selector[0] == '#') {
                std::string id_name = rule.selector.substr(1);
                bool id_exists = false;
                for(const auto& attr : node.attributes) {
                    if (attr.first == "id") { id_exists = true; break; }
                }
                if (!id_exists) {
                    node.addAttribute("id", id_name);
                }
            }
        }

        std::string parent_selector;
        for(const auto& attr : node.attributes) { if (attr.first == "class") { parent_selector = "." + attr.second; break; } }
        if (parent_selector.empty()) {
            for(const auto& attr : node.attributes) { if (attr.first == "id") { parent_selector = "#" + attr.second; break; } }
        }

        for (auto& rule : style_node->rules) {
            std::string final_selector = rule.selector;
            size_t pos = final_selector.find('&');
            if (pos != std::string::npos) {
                if (parent_selector.empty()) throw std::runtime_error("Cannot use '&' selector on an element with no class or id to reference.");
                final_selector.replace(pos, 1, parent_selector);
            }
            global_css += final_selector + " {\n";
            for (const auto& prop : rule.properties) { global_css += "  " + prop.first + ": " + prop.second + ";\n"; }
            global_css += "}\n";
        }
    }

    // --- Generation pass for this node ---
    output += indent_str + "<" + node.tagName;

    for (const auto& attr : node.attributes) {
        output += " " + attr.first + "=\"" + attr.second + "\"";
    }

    if (style_node) {
        std::string inline_style;
        for (const auto& prop : style_node->inline_properties) {
            inline_style += prop.first + ":" + prop.second + ";";
        }
        if (!inline_style.empty()) {
            output += " style=\"" + inline_style + "\"";
        }
    }

    output += ">\\n";

    for (auto& child : node.children) {
        if (child->getType() != NodeType::Style) {
            generateNode(*child, output, indent + 1);
        }
    }

    output += indent_str + "</" + node.tagName + ">\\n";
}

void Generator::generateTextNode(const TextNode& node, std::string& output, int indent) {
    std::string indent_str(indent * 2, ' ');
    output += indent_str + node.content + "\\n";
}

} // namespace CHTL
