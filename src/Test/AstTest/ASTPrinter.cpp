#include "ASTPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/CommentNode.h"
#include "../../CHTL/CHTLNode/AttributeNode.h"

namespace CHTL {

std::string ASTPrinter::print(NodePtr& node) {
    node->accept(*this);
    return out.str();
}

void ASTPrinter::parenthesize(const std::string& name, const std::vector<NodePtr>& nodes) {
    out << "(" << name;
    for (const auto& node : nodes) {
        out << " ";
        node->accept(*this);
    }
    out << ")";
}

void ASTPrinter::visit(ElementNode& node) {
    // A bit of a hack to visit attributes first if they exist
    std::stringstream temp_out;
    for (const auto& attr : node.attributes) {
        attr->accept(*this);
        temp_out << " " << out.str();
        out.str(""); // clear the stream
    }

    out << "(" << node.tagName << temp_out.str();

    for (const auto& child : node.children) {
        out << " ";
        child->accept(*this);
    }

    out << ")";
}

void ASTPrinter::visit(TextNode& node) {
    out << "(text \"" << node.content << "\")";
}

void ASTPrinter::visit(CommentNode& node) {
    out << "(comment \"" << node.content << "\")";
}

void ASTPrinter::visit(AttributeNode& node) {
    out << "(attr " << node.key << "=\"" << node.value << "\")";
}

} // namespace CHTL
