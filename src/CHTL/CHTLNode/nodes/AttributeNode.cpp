#include "AttributeNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

AttributeNode::AttributeNode(const String& name, const String& value,
                             const SourceRange& range)
    : BaseNode(NodeType::Attribute, range), name_(name), value_(value) {}

void AttributeNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String AttributeNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Attribute(" << name_ << "=\"" << value_ << "\")";
    return ss.str();
}

SharedPtr<BaseNode> AttributeNode::clone() const {
    return std::make_shared<AttributeNode>(name_, value_, range_);
}

} // namespace CHTL
