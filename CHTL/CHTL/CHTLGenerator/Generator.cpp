#include "Generator.h"
#include "../CHTLNode/BaseNode.h"

std::string Generator::generate(BaseNode& root) {
    output.str(""); // Clear any previous output
    output.clear(); // Clear any error flags
    root.accept(*this);
    return output.str();
}

void Generator::visit(ElementNode& node) {
    // A simple pretty-printing for readability
    output << "<" << node.getTagName() << ">";

    // For single-child text nodes, don't add newlines.
    bool simpleTextChild = node.getChildren().size() == 1 && dynamic_cast<TextNode*>(node.getChildren().front().get());

    if (!simpleTextChild && !node.getChildren().empty()) {
        output << std::endl;
    }

    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    if (!simpleTextChild && !node.getChildren().empty()) {
        output << std::endl;
    }

    output << "</" << node.getTagName() << ">";

    // Add a newline after top-level elements for spacing
    // This is a simplification; proper indentation is more complex.
    output << std::endl;
}

void Generator::visit(TextNode& node) {
    // In a real implementation, we would escape HTML special characters here.
    // e.g., '<' becomes '&lt;', '&' becomes '&amp;', etc.
    output << node.getContent();
}
