#include "ScriptNode.h"
#include <iostream>

namespace CHTL {

void ScriptNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "script {" << std::endl;

    for (int i = 0; i < indent + 1; ++i) {
        std::cout << "  ";
    }
    std::cout << content << std::endl;

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

} // namespace CHTL