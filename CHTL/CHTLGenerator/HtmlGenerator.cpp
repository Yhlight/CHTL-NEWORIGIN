#include "HtmlGenerator.h"

std::string HtmlGenerator::generate(Node& root) {
    ss.str(""); // Clear the string stream
    ss.clear();
    indentLevel = 0;
    root.accept(*this);
    return ss.str();
}

void HtmlGenerator::visit(RootNode& node) {
    for (const auto& child : node.getChildren()) {
        // We only generate code for top-level elements, not template definitions.
        if (dynamic_cast<ElementNode*>(child.get())) {
            child->accept(*this);
        }
    }
}

void HtmlGenerator::visit(ElementNode& node) {
    // Self-closing tags like <meta>, <img>, <br> etc.
    const std::vector<std::string> selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isSelfClosing = false;
    for(const auto& tag : selfClosingTags) {
        if (node.getTagName() == tag) {
            isSelfClosing = true;
            break;
        }
    }

    indent();
    ss << "<" << node.getTagName();

    // Regular attributes
    for (const auto& attr : node.getAttributes()) {
        ss << " " << attr->getName() << "=\"" << attr->getValue() << "\"";
    }

    // Style properties
    if (!node.getStyleStatements().empty()) {
        ss << " style=\"";
        for (const auto& stmt : node.getStyleStatements()) {
            if (auto style = dynamic_cast<const StylePropertyNode*>(stmt.get())) {
                ss << style->getProperty() << ":" << style->getValue() << ";";
            }
        }
        ss << "\"";
    }

    if (isSelfClosing) {
        ss << " />\n";
        return;
    }

    ss << ">\n";

    indentLevel++;
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    indentLevel--;

    indent();
    ss << "</" << node.getTagName() << ">\n";
}

void HtmlGenerator::visit(TextNode& node) {
    indent();
    ss << node.getContent() << "\n";
}

void HtmlGenerator::indent() {
    ss << std::string(indentLevel * 2, ' ');
}
