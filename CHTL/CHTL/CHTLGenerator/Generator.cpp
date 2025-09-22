#include "Generator.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"

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
    ss << "<" << node.tagName;
    // Attribute generation will go here
    ss << ">";

    // Visit children
    for (const auto& child : node.children) {
        if (child) {
            child->accept(*this);
        }
    }
    ss << "</" << node.tagName << ">";
}

void Generator::visit(const TextNode& node) {
    // In a real implementation, you would escape HTML special characters here.
    ss << node.value;
}

} // namespace CHTL
