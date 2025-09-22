#include "Generator.h"
#include "../CHTLNode/StyleNode.h"
#include <vector>
#include <iostream>
#include <string>

// Helper function to escape special HTML characters
std::string escapeHTML(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

Generator::Generator() {}

std::string Generator::Generate(Program* program) {
    output.str(""); // Clear the stream
    output.clear();
    for (const auto& stmt : program->statements) {
        visit(stmt.get());
    }
    return output.str();
}

void Generator::visit(Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::ELEMENT_DECLARATION:
            visitElementNode(static_cast<ElementNode*>(node));
            break;
        case NodeType::TEXT_DECLARATION:
            visitTextNode(static_cast<TextNode*>(node));
            break;
        case NodeType::STYLE_DECLARATION:
            visitStyleNode(static_cast<StyleNode*>(node));
            break;
        // Other node types can be handled here as they are added.
        default:
            // For now, we ignore nodes we don't know how to generate.
            break;
    }
}

void Generator::visitStyleNode(StyleNode* node) {
    output << "<style>" << node->content << "</style>";
}

void Generator::visitElementNode(ElementNode* node) {
    // Separate attributes from child nodes
    std::vector<AttributeNode*> attributes;
    std::vector<Node*> children;
    if (node->body) {
        for (const auto& stmt : node->body->statements) {
            if (stmt->getType() == NodeType::ATTRIBUTE_DECLARATION) {
                attributes.push_back(static_cast<AttributeNode*>(stmt.get()));
            } else {
                children.push_back(stmt.get());
            }
        }
    }

    // Write opening tag and attributes
    output << "<" << node->tagName;
    for (const auto& attr : attributes) {
        output << " " << attr->tokenLiteral() << "=\"";
        if (attr->value) {
            // We assume the value is a LiteralNode for now
            if (attr->value->getType() == NodeType::LITERAL) {
                 output << escapeHTML(static_cast<LiteralNode*>(attr->value.get())->value);
            }
        }
        output << "\"";
    }
    output << ">";

    // Visit children
    for (const auto& child : children) {
        visit(child);
    }

    // Write closing tag
    output << "</" << node->tagName << ">";
}

void Generator::visitTextNode(TextNode* node) {
    output << escapeHTML(node->value);
}

std::string Generator::visitLiteralNode(LiteralNode* node) {
    // This is used when an expression needs to be resolved to a string.
    return node->value;
}
