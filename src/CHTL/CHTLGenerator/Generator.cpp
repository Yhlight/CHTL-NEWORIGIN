#include "Generator.h"

std::string Generator::generate(BaseNode& root) {
    root.accept(*this);
    return output.str();
}

void Generator::visit(ElementNode& node) {
    if (node.tagName == "root") {
        for (auto& child : node.children) {
            child->accept(*this);
        }
        return;
    }

    indent();
    output << "<" << node.tagName;

    for (auto& attr : node.attributes) {
        attr->accept(*this);
    }

    if (node.children.empty()) {
        output << " />\n";
        return;
    }

    output << ">\n";
    indentLevel++;

    for (auto& child : node.children) {
        child->accept(*this);
    }

    indentLevel--;
    indent();
    output << "</" << node.tagName << ">\n";
}

void Generator::visit(TextNode& node) {
    indent();
    output << node.content << "\n";
}

void Generator::visit(CommentNode& node) {
    indent();
    output << "<!-- " << node.content << " -->\n";
}

void Generator::visit(AttributeNode& node) {
    output << " " << node.name << "=\"" << node.value << "\"";
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  ";
    }
}