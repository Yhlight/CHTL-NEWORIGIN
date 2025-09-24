#include "TextNode.h"
#include "../../Util/AstVisitor.h" // Corrected path

TextNode::TextNode(const std::string& content) : content(content) {}

void TextNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

const std::string& TextNode::getContent() const {
    return content;
}
