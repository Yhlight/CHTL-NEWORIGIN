#include "TextNode.h"
#include "AstVisitor.h"

namespace CHTL {

TextNode::TextNode(const std::string& text) : text(text) {}

void TextNode::Accept(AstVisitor& visitor) {
    visitor.Visit(*this);
}

} // namespace CHTL
