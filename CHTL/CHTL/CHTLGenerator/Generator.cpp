#include "Generator.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h" // Add missing include

namespace CHTL {

std::string Generator::generate(const BaseNode& root) {
    ss.str(""); // Clear the stringstream buffer
    ss.clear(); // Clear any error flags
    root.accept(*this);
    return ss.str();
}

void Generator::visit(const ProgramNode& node) {
    for (const auto& child : node.children) {
        if (child) {
            child->accept(*this);
        }
    }
}

void Generator::visit(const ElementNode& node) {
    // 1. Find StyleNode and build the inline style string
    std::string style_string;
    const StyleNode* style_node = nullptr;
    for (const auto& child : node.children) {
        // Use dynamic_cast to check if the child is a StyleNode
        if (auto sn = dynamic_cast<const StyleNode*>(child.get())) {
            style_node = sn;
            std::stringstream style_ss;
            for (const auto& prop : sn->properties) {
                style_ss << prop.key << ":" << prop.value << ";";
            }
            style_string = style_ss.str();
            break; // Assume only one style block per element for now
        }
    }

    // 2. Print opening tag with attributes and the new style attribute
    ss << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        ss << " " << attr.key << "=\"" << attr.value << "\"";
    }

    if (!style_string.empty()) {
        ss << " style=\"" << style_string << "\"";
    }

    ss << ">";

    // 3. Visit children, skipping the StyleNode so it's not rendered twice
    for (const auto& child : node.children) {
        if (child && child.get() != style_node) { // Skip the style node
            child->accept(*this);
        }
    }
    ss << "</" << node.tagName << ">";
}

void Generator::visit(const TextNode& node) {
    // In a real implementation, you would escape HTML special characters here.
    ss << node.value;
}

void Generator::visit(const StyleNode& node) {
    // This is intentionally left empty.
    // The StyleNode is processed by the ElementNode's visit method
    // to generate an inline style attribute. It does not produce direct output itself.
}

void Generator::visit(const TemplateStyleNode& node) {
    // A template definition produces no direct output in the final document.
    // It is only used for lookups by the parser/context.
    // This method is intentionally empty.
}

} // namespace CHTL
