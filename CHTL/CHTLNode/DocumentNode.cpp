#include "DocumentNode.h"
#include <iostream>

namespace CHTL {

void DocumentNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "DocumentNode" << (useHtml5 ? " (use html5)" : "") << std::endl;
    for (const auto& child : children) {
        child->print(indent + 1);
    }
}

} // namespace CHTL