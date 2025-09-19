#include "Generator.h"
#include "../CHTLNode/BaseNode.h"

std::string Generator::generate(BaseNode& root) {
    output.str(""); // Clear any previous output
    output.clear(); // Clear any error flags
    root.accept(*this);
    return output.str();
}

#include "../CHTLNode/StyleNode.h"

void Generator::visit(ElementNode& node) {
    output << "<" << node.getTagName();

    // Make a mutable copy of attributes to add styles from the style block.
    auto attributes = node.getAttributes();
    if (node.getStyleNode()) {
        // TODO: Merge with existing style attribute if it exists.
        // For now, this will overwrite a manually-set style attribute.
        attributes["style"] = node.getStyleNode()->getContent();
    }

    // Add attributes
    for (const auto& attr : attributes) {
        // In a real implementation, attribute values should be escaped.
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    output << ">";

    // For single-child text nodes, don't add newlines.
    bool simpleTextChild = node.getChildren().size() == 1 && dynamic_cast<TextNode*>(node.getChildren().front().get());

    if (!simpleTextChild && !node.getChildren().empty()) {
        output << "\n";
    }

    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    if (!simpleTextChild && !node.getChildren().empty()) {
        output << "\n";
    }

    output << "</" << node.getTagName() << ">" << "\n";
}

void Generator::visit(TextNode& node) {
    // In a real implementation, we would escape HTML special characters here.
    // e.g., '<' becomes '&lt;', '&' becomes '&amp;', etc.
    output << node.getContent();
}

void Generator::visit(CommentNode& node) {
    output << "<!-- " << node.getContent() << " -->\n";
}
