#include "Generator.h"
#include <iostream> // For potential debugging

namespace CHTL {

// Initialize the set of self-closing tags.
const std::set<std::string> Generator::selfClosingTags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

std::string Generator::generate(ProgramNode& root) {
    // Clear any previous output
    output.str("");
    output.clear();
    root.accept(*this);
    return output.str();
}

void Generator::visit(ProgramNode& node) {
    for (auto const& child : node.children) {
        if (child) {
            child->accept(*this);
        }
    }
}

void Generator::visit(ElementNode& node) {
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.key << "=\"";
        std::string value = attr.value.lexeme;
        // If the attribute value was a quoted string, strip the quotes.
        if (attr.value.type == TokenType::STRING) {
            if (value.length() >= 2) {
                value = value.substr(1, value.length() - 2);
            }
        }
        output << value << "\"";
    }

    // Check if the tag is self-closing
    if (selfClosingTags.count(node.tagName)) {
        output << " />";
        // Self-closing tags should not have children, but we handle it gracefully.
        return;
    }

    output << ">";

    // Visit all children to generate their content
    for (auto const& child : node.children) {
        if (child) {
            child->accept(*this);
        }
    }

    output << "</" << node.tagName << ">";
}

void Generator::visit(TextNode& node) {
    std::string content = node.content.lexeme;
    // If the text content was a quoted string, strip the quotes.
    if (node.content.type == TokenType::STRING) {
        if (content.length() >= 2) {
            content = content.substr(1, content.length() - 2);
        }
    }
    output << content;
}

} // namespace CHTL
