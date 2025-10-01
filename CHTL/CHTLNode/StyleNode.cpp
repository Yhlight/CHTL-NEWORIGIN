#include "StyleNode.h"
#include <iostream>

namespace CHTL {

void StyleNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "style {" << std::endl;

    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }

    for (const auto& rule : rules) {
        rule->print(indent + 1);
    }

    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

} // namespace CHTL