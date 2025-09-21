#include "Generator.h"
#include <iostream>

namespace CHTL {

Generator::Generator(const std::vector<NodePtr>& nodes, const std::map<std::string, std::shared_ptr<TemplateNode>>& templates)
    : rootNodes(nodes), templateTable(templates) {}

std::string Generator::generate() {
    for (const auto& node : rootNodes) {
        collectCssRules(node);
    }

    for (const auto& node : rootNodes) {
        visit(node);
    }
    return output.str();
}

void Generator::collectCssRules(const NodePtr& node) {
    if (!node) return;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        if (element->styleBlock) {
            for (const auto& rule : element->styleBlock->rules) {
                globalCssRules.push_back(rule);
            }
        }
        for (const auto& child : element->children) {
            collectCssRules(child);
        }
    }
}

void Generator::generateCss() {
    if (globalCssRules.empty()) return;

    indent();
    output << "<style>\n";
    indentLevel++;

    for (const auto& rule : globalCssRules) {
        indent();
        output << rule->selector << " {\n";
        indentLevel++;
        for (const auto& prop : rule->properties) {
            indent();
            output << prop.name << ": " << prop.value << ";\n";
        }
        indentLevel--;
        indent();
        output << "}\n";
    }

    indentLevel--;
    indent();
    output << "</style>\n";
}


void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}

void Generator::visit(const NodePtr& node) {
    if (!node) return;
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(text);
    } else if (auto usage = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
        visitTemplateUsageNode(usage);
    }
    // Add cases for other node types here later
}

void Generator::visitTemplateUsageNode(const std::shared_ptr<TemplateUsageNode>& node) {
    if (templateTable.count(node->name)) {
        auto templateNode = templateTable.at(node->name);
        if (templateNode->templateType == TemplateType::ELEMENT) {
            const auto& body = std::get<std::vector<NodePtr>>(templateNode->body);
            for (const auto& bodyNode : body) {
                visit(bodyNode);
            }
        }
        // Handle other template types if necessary, though @Style is handled by parser
    } else {
        // Handle error: template not found
        output << "<!-- Error: Template '" << node->name << "' not found. -->\n";
    }
}


void Generator::visitElementNode(const std::shared_ptr<ElementNode>& node) {
    indent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr.name << "=\"" << attr.value << "\"";
    }

    if (node->styleBlock) {
        std::stringstream style_ss;
        // First, resolve and add properties from template usages
        for (const auto& usage : node->styleBlock->templateUsages) {
            if (templateTable.count(usage->name)) {
                auto templateNode = templateTable.at(usage->name);
                if (templateNode->templateType == TemplateType::STYLE) {
                    const auto& props = std::get<std::vector<StyleProperty>>(templateNode->body);
                    for (const auto& prop : props) {
                        style_ss << prop.name << ": " << prop.value << ";";
                    }
                }
            }
        }

        // Then, add the inline properties (they can override template properties)
        for (const auto& prop : node->styleBlock->inlineProperties) {
            style_ss << prop.name << ": " << prop.value << ";";
        }

        if (style_ss.str().length() > 0) {
            output << " style=\"" << style_ss.str() << "\"";
        }
    }

    if (node->children.empty()) {
        // Self-closing tag for simplicity, though not all tags are self-closing in HTML
        // A proper implementation would have a list of void elements (area, base, br, etc.)
        output << " />\n";
    } else {
        output << ">\n";
        indentLevel++;

        for (const auto& child : node->children) {
            visit(child);
        }

        if (node->tagName == "head") {
            generateCss();
        }

        indentLevel--;
        indent();
        output << "</" << node->tagName << ">\n";
    }
}

void Generator::visitTextNode(const std::shared_ptr<TextNode>& node) {
    indent();
    output << node->text << "\n";
}

} // namespace CHTL
