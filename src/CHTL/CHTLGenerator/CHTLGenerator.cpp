#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include <iostream>
#include <string>
#include <map>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {}

CompilationOutput CHTLGenerator::generate(const ElementNode& root) {
    CompilationOutput output;
    generateElement(root, output.html, output.css, 0);
    return output;
}

void CHTLGenerator::generateElement(const ElementNode& element, std::string& html_output, std::string& css_output, int indentation) {
    std::string indent(indentation * 2, ' ');

    // --- Pre-process style information ---
    std::string style_string;
    std::map<std::string, std::string> attributes_to_add = {};
    std::string contextual_selector = "";

    for (const auto& attr : element.attributes) {
        attributes_to_add[attr.name] = attr.value;
    }

    // First pass over style nodes to determine the contextual selector and auto-add attributes
    for (const auto& child : element.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& rule : styleNode->rules) {
                if (rule.selector[0] == '.') {
                    std::string class_name = rule.selector.substr(1);
                    if (attributes_to_add.find("class") == attributes_to_add.end()) {
                        attributes_to_add["class"] = class_name;
                    } else {
                        attributes_to_add["class"] += " " + class_name;
                    }
                } else if (rule.selector[0] == '#') {
                    attributes_to_add["id"] = rule.selector.substr(1);
                }
            }
        }
    }

    // Determine contextual selector (ID > class)
    if (attributes_to_add.count("id")) {
        contextual_selector = "#" + attributes_to_add["id"];
    } else if (attributes_to_add.count("class")) {
        // Just use the first class for the context
        contextual_selector = "." + attributes_to_add["class"].substr(0, attributes_to_add["class"].find(" "));
    }


    // Second pass to generate styles and CSS rules
    for (const auto& child : element.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // Handle inline properties
            for (const auto& prop : styleNode->inline_properties) {
                style_string += prop.name + ":" + prop.value + ";";
            }
            // Handle rules
            for (const auto& rule : styleNode->rules) {
                std::string final_selector = rule.selector;
                if (final_selector[0] == '&') {
                    if (!contextual_selector.empty()) {
                        final_selector.replace(0, 1, contextual_selector);
                    } else {
                        // Warning: '&' used without a class or id on the element.
                        // For now, we'll just remove the '&'.
                        final_selector.erase(0, 1);
                    }
                }

                css_output += final_selector + " {\n";
                for (const auto& prop : rule.properties) {
                    css_output += "  " + prop.name + ": " + prop.value + ";\n";
                }
                css_output += "}\n";
            }
        }
    }


    if (!style_string.empty()) {
        attributes_to_add["style"] = style_string;
    }
    // --- End of style pre-processing ---


    // --- Generate opening tag with all attributes ---
    html_output += indent + "<" + element.tagName;
    for (const auto& pair : attributes_to_add) {
        html_output += " " + pair.first + "=\"" + pair.second + "\"";
    }

    // --- Check for children ---
    bool has_non_style_children = false;
    for (const auto& child : element.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            has_non_style_children = true;
            break;
        }
    }

    if (!has_non_style_children) {
        html_output += " />\n";
        return;
    }

    html_output += ">\n";

    // --- Generate children ---
    for (const auto& child : element.children) {
        if (auto* el = dynamic_cast<ElementNode*>(child.get())) {
            generateElement(*el, html_output, css_output, indentation + 1);
        } else if (auto* txt = dynamic_cast<TextNode*>(child.get())) {
            html_output += std::string((indentation + 1) * 2, ' ') + txt->content + "\n";
        }
    }

    // --- Generate closing tag ---
    html_output += indent + "</" + element.tagName + ">\n";
}

} // namespace CHTL
