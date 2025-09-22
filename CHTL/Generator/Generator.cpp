#include "Generator.h"
#include "CHTL/Node/ElementNode.h"
#include "CHTL/Node/TextNode.h"
#include "CHTL/Node/CommentNode.h"
#include "CHTL/Evaluator/Evaluator.h"
#include <regex>

std::string Generator::generate(const ParseResult& result) {
    output.str(""); // Clear the stream

    // 1. Render global styles
    if (!result.global_styles.empty()) {
        output << "<style>\n";
        Evaluator dummy_evaluator(result.ast, ElementNode("", {}, {}, {})); // Dummy context for now
        for (const auto& rule : result.global_styles) {
            std::string selector = rule.selector;
            if (selector.find('&') != std::string::npos) {
                selector = std::regex_replace(selector, std::regex("&"), rule.context_selector);
            }
            output << "  " << selector << " {\n";
            for (const auto& prop : rule.properties) {
                // Evaluating global properties is complex because we don't have a clear 'current_element'.
                // For now, we will just evaluate them with a dummy context. This will only work for non-referential expressions.
                Value val = dummy_evaluator.evaluate(*prop.value);
                output << "    " << prop.key << ": " << val.to_css_string() << ";\n";
            }
            output << "  }\n";
        }
        output << "</style>\n\n";
    }

    // 2. Render AST
    for (const auto& node : result.ast) {
        if (node) {
            node->accept(*this);
            output << "\n";
        }
    }
    return output.str();
}

void Generator::indent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "  ";
    }
}

void Generator::visit(const ElementNode& node) {
    indent();
    output << "<" << node.tag_name;

    for (const auto& attr : node.attributes) {
        output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    if (!node.styles.empty()) {
        output << " style=\"";
        // Here we need to create the evaluator with the correct context
        Evaluator evaluator(NodeList{}, node); // The root AST is not needed for local refs
        for (const auto& prop : node.styles) {
            Value val = evaluator.evaluate(*prop.value);
            output << prop.key << ":" << val.to_css_string() << ";";
        }
        output << "\"";
    }

    output << ">";

    if (!node.children.empty()) {
        output << "\n";
        indent_level++;
        for (const auto& child : node.children) {
            child->accept(*this);
            output << "\n";
        }
        indent_level--;
        indent();
    }

    output << "</" << node.tag_name << ">";
}

void Generator::visit(const TextNode& node) {
    indent();
    output << node.text;
}

void Generator::visit(const CommentNode& node) {
    indent();
    output << "<!-- " << node.comment << " -->";
}
