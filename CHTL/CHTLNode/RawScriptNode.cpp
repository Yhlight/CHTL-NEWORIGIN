#include "RawScriptNode.h"
#include <iostream>

namespace CHTL {

RawScriptNode::RawScriptNode(const std::string& script)
    : BaseNode(), m_script(script) {}

void RawScriptNode::print(int level) const {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
    std::cout << "RawScriptNode: " << m_script << std::endl;
}

const std::string& RawScriptNode::getScript() const {
    return m_script;
}

} // namespace CHTL