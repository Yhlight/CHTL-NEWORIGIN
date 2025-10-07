#include "ImportNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

ImportNode::ImportNode(const String& path, const SourceRange& range)
    : BaseNode(NodeType::Import, range), path_(path) {}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ImportNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Import(" << path_;
    if (!alias_.empty()) {
        ss << " as " << alias_;
    }
    ss << ")";
    return ss.str();
}

SharedPtr<BaseNode> ImportNode::clone() const {
    auto node = std::make_shared<ImportNode>(path_, range_);
    node->alias_ = alias_;
    return node;
}

} // namespace CHTL
