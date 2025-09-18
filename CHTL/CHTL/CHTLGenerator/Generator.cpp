#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include <stdexcept>

std::string Generator::generate(NodePtr node) {
    // Ensure the stringstream is clear before starting generation.
    m_out.str("");
    m_out.clear();
    visit(node);
    return m_out.str();
}

void Generator::visit(NodePtr node) {
    // Use dynamic_pointer_cast to determine the node's concrete type and call the appropriate visitor.
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(element.get());
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(text.get());
    } else if (auto comment = std::dynamic_pointer_cast<CommentNode>(node)) {
        visitCommentNode(comment.get());
    } else {
        // This should not be reached with the current AST structure.
        // It's good practice to have a fallback for unexpected node types.
        throw std::runtime_error("Unknown node type encountered in Generator.");
    }
}

void Generator::visitElementNode(ElementNode* node) {
    // The "program" node is a virtual root created by the parser to hold all top-level statements.
    // It should not be rendered in the final HTML.
    const bool isProgramRoot = (node->m_tagName == "program");

    // The "text" element is also a special case. We only want to render its content,
    // not the "<text>" tags themselves.
    const bool isTextElement = (node->m_tagName == "text");

    if (!isProgramRoot && !isTextElement) {
        m_out << "<" << node->m_tagName;

        // Render attributes
        for (const auto& attr : node->m_attributes) {
            m_out << " " << attr.first << "=\"" << attr.second << "\"";
        }

        // Render inline styles
        if (!node->m_inlineStyles.empty()) {
            m_out << " style=\"";
            for (const auto& style : node->m_inlineStyles) {
                m_out << style.first << ": " << style.second << ";";
            }
            m_out << "\"";
        }

        m_out << ">";
    }

    // Recursively visit all children of the current element.
    for (const auto& child : node->m_children) {
        visit(child);
    }

    if (!isProgramRoot && !isTextElement) {
        m_out << "</" << node->m_tagName << ">";
    }
}

void Generator::visitTextNode(TextNode* node) {
    // For a text node, simply append its string value to the output.
    m_out << node->m_value;
}

void Generator::visitCommentNode(CommentNode* node) {
    m_out << "<!-- " << node->m_value << " -->";
}
