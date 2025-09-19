#include "Generator.h"
#include "../CHTLNode/BaseNode.h"

std::string Generator::generate(BaseNode& root) {
    output.str(""); // Clear any previous output
    output.clear(); // Clear any error flags
    root.accept(*this);
    return output.str();
}

void Generator::visit(ElementNode& node) {
    output << "<" << node.getTagName();

    // Add attributes
    for (const auto& attr : node.getAttributes()) {
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
