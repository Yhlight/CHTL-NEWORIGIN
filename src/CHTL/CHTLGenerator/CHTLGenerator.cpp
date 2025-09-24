#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/AttributeNode.h"
#include "CHTLNode/StyleBlockNode.h"
#include "CHTLNode/SelectorBlockNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include <stdexcept>

namespace CHTL {

std::string CHTLGenerator::generate(BaseNode& root) {
    _output.str("");
    _global_styles.str("");
    _indent_level = 0;
    generateNode(root);
    return _output.str();
}

std::string CHTLGenerator::getGlobalStyles() const {
    return _global_styles.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < _indent_level; ++i) {
        _output << "  ";
    }
}

void CHTLGenerator::generateNode(BaseNode& node) {
    if (auto* element = dynamic_cast<ElementNode*>(&node)) {
        generateElement(*element);
    } else if (auto* text = dynamic_cast<TextNode*>(&node)) {
        generateText(*text);
    }
}

void CHTLGenerator::generateSelectorBlock(const SelectorBlockNode& node) {
    _global_styles << "    " << node.selector << " {\n";
    for (const auto& prop : node.properties) {
        _global_styles << "      " << prop->property << ": " << prop->value << ";\n";
    }
    _global_styles << "    }\n";
}

void CHTLGenerator::generateElement(ElementNode& node) {
    indent();
    _output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        _output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    if (node.style) {
        std::stringstream inline_style_ss;
        bool has_inline_styles = false;
        for (const auto& rule : node.style->rules) {
            if (const auto* prop = dynamic_cast<const StylePropertyNode*>(rule.get())) {
                if (has_inline_styles) {
                    inline_style_ss << " ";
                }
                inline_style_ss << prop->property << ": " << prop->value << ";";
                has_inline_styles = true;
            } else if (const auto* selector = dynamic_cast<const SelectorBlockNode*>(rule.get())) {
                generateSelectorBlock(*selector);
            }
        }
        if (has_inline_styles) {
            _output << " style=\"" << inline_style_ss.str() << "\"";
        }
    }

    _output << ">\n";

    _indent_level++;
    for (auto& child : node.children) {
        generateNode(*child);
    }
    _indent_level--;

    indent();
    _output << "</" << node.tagName << ">\n";
}

void CHTLGenerator::generateText(TextNode& node) {
    indent();
    _output << node.content << "\n";
}

} // namespace CHTL
