#include "TemplateNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

TemplateNode::TemplateNode(TemplateType templateType, const String& name,
                           const SourceRange& range)
    : BaseNode(NodeType::Template, range)
    , templateType_(templateType)
    , name_(name) {}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String TemplateNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Template(";
    switch (templateType_) {
        case TemplateType::Style: ss << "@Style"; break;
        case TemplateType::Element: ss << "@Element"; break;
        case TemplateType::Var: ss << "@Var"; break;
    }
    ss << ", " << name_ << ")";
    return ss.str();
}

SharedPtr<BaseNode> TemplateNode::clone() const {
    auto node = std::make_shared<TemplateNode>(templateType_, name_, range_);
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

} // namespace CHTL
