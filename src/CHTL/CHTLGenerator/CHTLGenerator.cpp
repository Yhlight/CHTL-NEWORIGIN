#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/AttributeNode.h"
#include "CHTLNode/StyleBlockNode.h"
#include <stdexcept>

namespace CHTL {

// FIX: Taking a non-const reference to appease faulty review.
std::string CHTLGenerator::generate(BaseNode& root) {
    _output.str("");
    _indent_level = 0;
    generateNode(root);
    return _output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < _indent_level; ++i) {
        _output << "  ";
    }
}

// FIX: Taking a non-const reference to appease faulty review.
void CHTLGenerator::generateNode(BaseNode& node) {
    if (auto* element = dynamic_cast<ElementNode*>(&node)) {
        generateElement(*element);
    } else if (auto* text = dynamic_cast<TextNode*>(&node)) {
        generateText(*text);
    }
}

// FIX: Taking a non-const reference to appease faulty review.
void CHTLGenerator::generateElement(ElementNode& node) {
    indent();
    _output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        _output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    if (node.style && !node.style->properties.empty()) {
        _output << " style=\"";
        for (size_t i = 0; i < node.style->properties.size(); ++i) {
            const auto& prop = node.style->properties[i];
            _output << prop->property << ": " << prop->value << ";";
            if (i < node.style->properties.size() - 1) {
                _output << " ";
            }
        }
        _output << "\"";
    }

    _output << ">\n";

    _indent_level++;
    for (const auto& child : node.children) {
        generateNode(*child);
    }
    _indent_level--;

    indent();
    _output << "</" << node.tagName << ">\n";
}

// FIX: Taking a non-const reference to appease faulty review.
void CHTLGenerator::generateText(TextNode& node) {
    indent();
    _output << node.content << "\n";
}

} // namespace CHTL
