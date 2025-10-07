#include "BaseNode.h"
#include <sstream>

namespace CHTL {

// BaseNode实现
BaseNode::BaseNode(NodeType type, const SourceRange& range)
    : type_(type), range_(range) {}

void BaseNode::addChild(SharedPtr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

String BaseNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "BaseNode(" << static_cast<int>(type_) << ")";
    return ss.str();
}

String BaseNode::indentString(int indent) const {
    return String(indent * 2, ' ');
}

} // namespace CHTL
