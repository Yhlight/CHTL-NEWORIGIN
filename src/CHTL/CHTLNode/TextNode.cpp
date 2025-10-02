/**
 * @file TextNode.cpp
 * @brief Implementation of TextNode
 */

#include "TextNode.h"

namespace CHTL {

TextNode::TextNode(const std::string& content)
    : BaseNode(NodeType::TEXT)
    , content_(content)
{
}

std::string TextNode::toHTML() const {
    return content_;
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string TextNode::toString() const {
    return "Text: \"" + content_ + "\"";
}

} // namespace CHTL
