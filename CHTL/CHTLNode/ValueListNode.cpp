#include "ValueListNode.h"
#include <iostream>

namespace CHTL {

void ValueListNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "ValueList (" << std::endl;
    for (const auto& value : values) {
        value->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << ")" << std::endl;
}

} // namespace CHTL