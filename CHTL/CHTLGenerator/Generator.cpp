#include "Generator.h"
#include <stdexcept>

namespace CHTL {

std::string Generator::Generate(const ElementNode& root) {
    std::stringstream ss;
    // The root node is a virtual container (__ROOT__), so we generate its children directly.
    for (const auto& child : root.children) {
        generateNode(*child, ss);
    }
    return ss.str();
}

void Generator::generateNode(const BaseNode& node, std::stringstream& ss) {
    // Use dynamic_cast to determine the concrete type of the node.
    if (const auto* element = dynamic_cast<const ElementNode*>(&node)) {
        generateNode(*element, ss);
    } else if (const auto* text = dynamic_cast<const TextNode*>(&node)) {
        generateNode(*text, ss);
    } else {
        // In a real application, you might want to log this or handle other node types.
        // For now, we can throw or ignore.
        throw std::runtime_error("Unknown node type encountered in generator.");
    }
}

void Generator::generateNode(const ElementNode& element, std::stringstream& ss) {
    // Don't render the virtual root element.
    if (element.tagName == "__ROOT__") {
        for (const auto& child : element.children) {
            generateNode(*child, ss);
        }
        return;
    }

    ss << "<" << element.tagName;

    for (const auto& attr : element.attributes) {
        ss << " " << attr->name << "=\"" << attr->value << "\"";
    }

    // For now, we assume no self-closing tags.
    ss << ">";

    for (const auto& child : element.children) {
        generateNode(*child, ss);
    }

    ss << "</" << element.tagName << ">";
}

void Generator::generateNode(const TextNode& text, std::stringstream& ss) {
    // For now, no HTML escaping.
    ss << text.content;
}

} // CHTL
