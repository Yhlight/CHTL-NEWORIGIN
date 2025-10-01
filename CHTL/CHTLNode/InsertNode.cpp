#include "InsertNode.h"
#include <iostream>

namespace CHTL {

InsertNode::InsertNode(InsertPosition position, const std::string& selector, std::vector<std::unique_ptr<BaseNode>> children)
    : m_position(position), m_selector(selector), m_children(std::move(children)) {}

void InsertNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::string pos_str;
    switch (m_position) {
        case InsertPosition::AFTER: pos_str = "after"; break;
        case InsertPosition::BEFORE: pos_str = "before"; break;
        case InsertPosition::REPLACE: pos_str = "replace"; break;
        case InsertPosition::AT_TOP: pos_str = "at top"; break;
        case InsertPosition::AT_BOTTOM: pos_str = "at bottom"; break;
    }
    std::cout << "Insert " << pos_str << " " << m_selector << " {" << std::endl;
    for (const auto& child : m_children) {
        child->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

InsertPosition InsertNode::getPosition() const {
    return m_position;
}

const std::string& InsertNode::getSelector() const {
    return m_selector;
}

const std::vector<std::unique_ptr<BaseNode>>& InsertNode::getChildren() const {
    return m_children;
}

} // namespace CHTL