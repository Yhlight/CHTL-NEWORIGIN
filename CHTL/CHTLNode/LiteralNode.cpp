#include "LiteralNode.h"
#include <iostream>

namespace CHTL {

void LiteralNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Literal: " << value.value << std::endl;
}


} // namespace CHTL