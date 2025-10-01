#include "DeleteNode.h"
#include <iostream>

namespace CHTL {

DeleteNode::DeleteNode(const std::vector<std::string>& targets) : m_targets(targets) {}

void DeleteNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Delete: ";
    for (size_t i = 0; i < m_targets.size(); ++i) {
        std::cout << m_targets[i] << (i < m_targets.size() - 1 ? ", " : "");
    }
    std::cout << std::endl;
}

const std::vector<std::string>& DeleteNode::getTargets() const {
    return m_targets;
}

} // namespace CHTL