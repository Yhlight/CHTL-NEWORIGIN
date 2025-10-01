#include "ScriptNode.h"
#include <iostream>

namespace CHTL {

void ScriptNode::addChild(std::unique_ptr<BaseNode> child) {
    m_children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<BaseNode>>& ScriptNode::getChildren() const {
    return m_children;
}

void ScriptNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "ScriptNode:" << std::endl;
    for (const auto& child : m_children) {
        child->print(indent + 1);
    }
}

} // namespace CHTL