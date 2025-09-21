#include "Generator.h"
#include <stdexcept>

namespace CHTL {

CompilationResult Generator::generate(const ElementNode& root) {
    m_result = CompilationResult();
    m_placeholder_count = 0;

    std::string html_output;
    generateNode(root, html_output, 0);
    m_result.html_output = html_output;

    return m_result;
}

void Generator::generateNode(const Node& node, std::string& output, int indentLevel) {
    switch (node.getType()) {
        case NodeType::Element:
            generateElement(static_cast<const ElementNode&>(node), output, indentLevel);
            break;
        case NodeType::Text:
            generateText(static_cast<const TextNode&>(node), output, indentLevel);
            break;
        case NodeType::StyleBlock:
            generateStyleBlock(static_cast<const StyleBlockNode&>(node), output, indentLevel);
            break;
        default:
            throw std::runtime_error("Unknown node type during generation.");
    }
}

void Generator::generateElement(const ElementNode& element, std::string& output, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');
    output += indent + "<" + element.tagName;

    for (const auto& attr : element.attributes) {
        output += " " + attr.first + "=\"" + attr.second + "\"";
    }

    if (element.children.empty() && element.tagName != "style") { // Don't self-close style tags
        output += ">\n";
    } else {
        output += ">\n";
        for (const auto& child : element.children) {
            generateNode(*child, output, indentLevel + 1);
        }
    }

    output += indent + "</" + element.tagName + ">\n";
}

void Generator::generateText(const TextNode& text, std::string& output, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');
    output += indent + text.content + "\n";
}

void Generator::generateStyleBlock(const StyleBlockNode& style_block, std::string& output, int indentLevel) {
    std::string placeholder = "__CSS_PLACEHOLDER_" + std::to_string(m_placeholder_count++) + "__";
    m_result.css_outputs[placeholder] = style_block.css_content;

    std::string indent(indentLevel * 2, ' ');
    output += indent + placeholder + "\n";
}


} // namespace CHTL
