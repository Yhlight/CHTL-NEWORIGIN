#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <iostream>

void CHTLGenerator::generate(const std::vector<std::shared_ptr<BaseNode>>& nodes, CHTLContext& context) {
    this->context = &context;
    for (const auto& node : nodes) {
        if (node) {
            node->accept(*this);
        }
    }
}

void CHTLGenerator::indent() {
    context->html_output << std::string(indent_level * 2, ' ');
}

void CHTLGenerator::visit(ElementNode& node) {
    indent();
    context->html_output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        context->html_output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    if (!node.inlineStyles.empty()) {
        context->html_output << " style=\"";
        for (auto it = node.inlineStyles.begin(); it != node.inlineStyles.end(); ++it) {
            context->html_output << it->first << ":" << it->second << ";";
        }
        context->html_output << "\"";
    }

    context->html_output << ">\n";

    indent_level++;
    for (const auto& child : node.children) {
        if (child) {
            child->accept(*this);
        }
    }
    indent_level--;

    indent();
    context->html_output << "</" << node.tagName << ">\n";

    // Add any global CSS rules to the context
    for (const auto& rule : node.globalCssRules) {
        context->css_output << rule.selector << " {\n";
        for (const auto& prop : rule.properties) {
            context->css_output << "  " << prop.first << ": " << prop.second << ";\n";
        }
        context->css_output << "}\n";
    }
}

void CHTLGenerator::visit(TextNode& node) {
    indent();
    context->html_output << node.content << "\n";
}

void CHTLGenerator::visit(CommentNode& node) {
    indent();
    context->html_output << "<!-- " << node.content << " -->\n";
}
