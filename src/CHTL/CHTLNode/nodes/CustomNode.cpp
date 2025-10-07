#include "CustomNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

CustomNode::CustomNode(CustomType customType, const String& name,
                       const SourceRange& range)
    : BaseNode(NodeType::Custom, range)
    , customType_(customType)
    , name_(name) {}

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String CustomNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Custom(";
    switch (customType_) {
        case CustomType::Style: ss << "@Style"; break;
        case CustomType::Element: ss << "@Element"; break;
        case CustomType::Var: ss << "@Var"; break;
    }
    ss << ", " << name_ << ")";
    return ss.str();
}

SharedPtr<BaseNode> CustomNode::clone() const {
    auto node = std::make_shared<CustomNode>(customType_, name_, range_);
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

} // namespace CHTL
