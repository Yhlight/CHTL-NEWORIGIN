#include "ScriptNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

ScriptNode::ScriptNode(const String& content, const SourceRange& range)
    : BaseNode(NodeType::Script, range), content_(content) {}

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ScriptNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Script";
    if (isLocal_) ss << "(local)";
    ss << " {" << content_.substr(0, 50);
    if (content_.length() > 50) ss << "...";
    ss << "}";
    return ss.str();
}

SharedPtr<BaseNode> ScriptNode::clone() const {
    auto node = std::make_shared<ScriptNode>(content_, range_);
    node->isLocal_ = isLocal_;
    return node;
}

} // namespace CHTL
