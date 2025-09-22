#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"

namespace CHTL {

std::string HtmlGenerator::generate(BaseNode* root) {
    output.str("");
    output.clear();
    root->accept(*this);
    return output.str();
}

void HtmlGenerator::visit(ElementNode& node) {
    // The "root" node is a special case, it's just a container for top-level elements
    // and should not be rendered itself.
    if (node.tagName == "root") {
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        return;
    }

    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " ";
        attr->accept(*this);
    }

    // Simple check for void elements - can be expanded
    const std::vector<std::string> voidElements = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isVoid = false;
    for(const auto& el : voidElements){
        if(node.tagName == el){
            isVoid = true;
            break;
        }
    }

    if (node.children.empty() && isVoid) {
        output << ">";
    } else {
        output << ">";
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        output << "</" << node.tagName << ">";
    }
}

void HtmlGenerator::visit(TextNode& node) {
    output << node.content;
}

void HtmlGenerator::visit(AttributeNode& node) {
    output << node.key << "=\"" << node.value << "\"";
}

} // namespace CHTL
