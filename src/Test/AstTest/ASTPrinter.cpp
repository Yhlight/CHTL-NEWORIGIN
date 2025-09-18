#include "ASTPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/AttributeNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include <sstream>

namespace CHTL {

std::string ASTPrinter::print(NodePtr& node) {
    if (!node) return "(null)";
    return node->accept(*this);
}

std::string ASTPrinter::visit(ElementNode& node) {
    std::stringstream ss;
    ss << "(" << node.tagName;

    for (const auto& attr : node.attributes) {
        ss << " " << attr->accept(*this);
    }

    for (const auto& child : node.children) {
        ss << " " << child->accept(*this);
    }

    ss << ")";
    return ss.str();
}

std::string ASTPrinter::visit(TextNode& node) {
    return "(text \"" + node.content + "\")";
}

std::string ASTPrinter::visit(CommentNode& node) {
    return "(comment \"" + node.content + "\")";
}

std::string ASTPrinter::visit(AttributeNode& node) {
    return "(attr " + node.key + "=\"" + node.value + "\")";
}

std::string ASTPrinter::visit(StyleNode& node) {
    std::stringstream ss;
    ss << "(style";
    for (const auto& prop : node.inline_properties) {
        ss << " (prop " << prop.key << ": " << prop.value << ")";
    }
    for (const auto& rule : node.nested_rules) {
        ss << " (rule " << rule.selector << " {";
        for (const auto& prop : rule.properties) {
            ss << " " << prop.key << ": " << prop.value << ";";
        }
        ss << " })";
    }
    ss << ")";
    return ss.str();
}

} // namespace CHTL
