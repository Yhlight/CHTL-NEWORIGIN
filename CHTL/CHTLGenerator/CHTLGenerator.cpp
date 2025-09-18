#include "CHTLGenerator.h"
#include "../ExpressionEvaluator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

namespace CHTL {

CHTLGenerator::CHTLGenerator() {
    // Initialize the set of HTML5 void elements.
    // These elements should not have a closing tag.
    m_voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
}

std::string CHTLGenerator::generate(const BaseNode& root) {
    m_output.str(""); // Clear any previous output
    m_indentationLevel = 0;
    root.accept(*this);
    return m_output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < m_indentationLevel; ++i) {
        m_output << "  "; // Use 2 spaces for indentation
    }
}

void CHTLGenerator::visit(const ElementNode& node) {
    // The "document_root" is a virtual node created by the parser to hold
    // all top-level elements. We don't render it; we just render its children.
    if (node.tagName == "document_root") {
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        return;
    }

    indent();
    m_output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        m_output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // Add inline styles from style {} blocks.
    if (!node.styles.empty()) {
        ExpressionEvaluator evaluator;
        m_output << " style=\"";
        for (size_t i = 0; i < node.styles.size(); ++i) {
            std::string value = evaluator.evaluate(*node.styles[i].value);
            m_output << node.styles[i].key << ": " << value << ";";
            if (i < node.styles.size() - 1) {
                m_output << " "; // Add space between properties for readability
            }
        }
        m_output << "\"";
    }

    if (m_voidElements.count(node.tagName)) {
        m_output << ">\n"; // Void elements in HTML5 don't need to be self-closed (e.g., <br>)
        return;
    }

    m_output << ">\n";

    m_indentationLevel++;
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    m_indentationLevel--;

    indent();
    m_output << "</" << node.tagName << ">\n";
}

void CHTLGenerator::visit(const TextNode& node) {
    indent();
    m_output << node.content << "\n";
}

void CHTLGenerator::visit(const CommentNode& node) {
    // Per CHTL spec, only 'GENERATOR' type comments are rendered into the output.
    if (node.type == CommentType::GENERATOR) {
        indent();
        m_output << "<!-- " << node.content << " -->\n";
    }
}

} // namespace CHTL
