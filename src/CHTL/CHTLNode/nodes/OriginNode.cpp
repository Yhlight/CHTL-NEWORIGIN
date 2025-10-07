#include "OriginNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

OriginNode::OriginNode(OriginType originType, const String& content,
                       const SourceRange& range)
    : BaseNode(NodeType::Origin, range)
    , originType_(originType)
    , content_(content) {}

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String OriginNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Origin(";
    switch (originType_) {
        case OriginType::Html: ss << "@Html"; break;
        case OriginType::Style: ss << "@Style"; break;
        case OriginType::JavaScript: ss << "@JavaScript"; break;
        case OriginType::Custom: ss << "@" << typeName_; break;
    }
    ss << ")";
    return ss.str();
}

SharedPtr<BaseNode> OriginNode::clone() const {
    auto node = std::make_shared<OriginNode>(originType_, content_, range_);
    node->typeName_ = typeName_;
    return node;
}

} // namespace CHTL
