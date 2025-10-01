#include "VariableAccessNode.h"
#include <iostream>

namespace CHTL {

void VariableAccessNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "VariableAccess: " << m_groupName << "(" << m_variableName << ")" << std::endl;
}

} // namespace CHTL