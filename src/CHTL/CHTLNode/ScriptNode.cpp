/**
 * @file ScriptNode.cpp
 * @brief Implementation of ScriptNode
 */

#include "ScriptNode.h"

namespace CHTL {

ScriptNode::ScriptNode()
    : BaseNode(NodeType::SCRIPT)
    , isLocal_(false)
{
}

std::string ScriptNode::toHTML() const {
    // Scripts typically go to separate file
    return "";
}

std::string ScriptNode::toJS() const {
    return content_ + "\n";
}

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

std::string ScriptNode::toString() const {
    std::string type = isLocal_ ? "Local Script" : "Global Script";
    return type;
}

} // namespace CHTL
