#include "EnhancedSelectorNode.h"
#include <iostream>

namespace CHTL {

EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector)
    : BaseNode(), m_selector(selector) {}

void EnhancedSelectorNode::print(int level) const {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
    std::cout << "EnhancedSelectorNode: " << m_selector << std::endl;
}

const std::string& EnhancedSelectorNode::getSelector() const {
    return m_selector;
}

} // namespace CHTL