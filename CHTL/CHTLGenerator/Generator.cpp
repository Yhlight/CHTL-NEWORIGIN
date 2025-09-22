#include "Generator.h"
#include <iostream>

// List of HTML tags that are self-closing (void elements)
const std::vector<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

bool isVoidElement(const std::string& tagName) {
    for (const auto& el : voidElements) {
        if (el == tagName) {
            return true;
        }
    }
    return false;
}

std::string Generator::generate(BaseNode& root) {
    output.str(""); // Clear previous output
    output.clear();
    indentLevel = 0;
    root.accept(*this);
    return output.str();
}

void Generator::visit(ElementNode& node) {
    // The "root" node is a virtual container, so we don't print it.
    // We only process its children.
    if (node.tagName == "root") {
        for (auto& child : node.children) {
            child->accept(*this);
        }
        return;
    }

    indent();
    output << "<" << node.tagName;

    // Print attributes
    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Print inline styles
    if (!node.inlineStyles.empty()) {
        output << " style=\"";
        for (const auto& style : node.inlineStyles) {
            output << style.first << ": " << style.second << ";";
        }
        output << "\"";
    }

    output << ">";

    if (isVoidElement(node.tagName)) {
        output << "\n";
        return;
    }

    // If there's only one child and it's a text node, print it on the same line.
    bool hasSingleTextChild = node.children.size() == 1 && dynamic_cast<TextNode*>(node.children[0].get());

    if (!hasSingleTextChild) {
        output << "\n";
        indentLevel++;
    }

    for (auto& child : node.children) {
        child->accept(*this);
    }

    if (!hasSingleTextChild) {
        indentLevel--;
        indent();
    }

    output << "</" << node.tagName << ">\n";
}

void Generator::visit(TextNode& node) {
    // If not inside a single-text-child element, indent the text.
    // This is a simplification; proper text formatting is complex.
    if (indentLevel > 0) {
       // indent(); // Decided against indenting raw text to avoid extra whitespace
    }
    output << node.content;
}

void Generator::visit(CommentNode& node) {
    indent();
    // The lexer captures the #, so we strip it.
    std::string content = node.content.substr(1);
    output << "<!--" << content << " -->\n";
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // 2 spaces for indentation
    }
}
