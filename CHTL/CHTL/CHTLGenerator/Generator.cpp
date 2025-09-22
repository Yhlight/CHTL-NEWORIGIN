#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleBlockNode.h"

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

    // Aggregate styles from any StyleBlockNode children
    std::stringstream style_stream;
    for (const auto& child : node.children) {
        if (auto* style_block = dynamic_cast<StyleBlockNode*>(child.get())) {
            for (const auto& prop : style_block->properties) {
                style_stream << prop->property << ":" << prop->value << ";";
            }
        }
    }

    std::string style_string = style_stream.str();
    if (!style_string.empty()) {
        output << " style=\"" << style_string << "\"";
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
            // Skip StyleBlockNodes as they've already been processed
            if (dynamic_cast<StyleBlockNode*>(child.get()) == nullptr) {
                child->accept(*this);
            }
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

void HtmlGenerator::visit(StyleBlockNode& node) {
    // This is intentionally left empty.
    // The ElementNode's visit method is responsible for finding StyleBlockNodes
    // and aggregating their properties into an inline style attribute.
    // The StyleBlockNode itself doesn't render anything directly.
}

void HtmlGenerator::visit(CssPropertyNode& node) {
    // This is also intentionally left empty, as the parent ElementNode
    // directly accesses the properties and formats them.
}

} // namespace CHTL
