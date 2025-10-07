#include "TextNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

TextNode::TextNode(const String& content, const SourceRange& range)
    : BaseNode(NodeType::Text, range), content_(content) {}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String TextNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Text(\"" << content_ << "\")";
    return ss.str();
}

SharedPtr<BaseNode> TextNode::clone() const {
    return std::make_shared<TextNode>(content_, range_);
}

} // namespace CHTL
