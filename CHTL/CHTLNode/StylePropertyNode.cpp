#include "StylePropertyNode.h"
#include <iostream>

namespace CHTL {

void StylePropertyNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << key << ": " << value << ";" << std::endl;
}

} // namespace CHTL