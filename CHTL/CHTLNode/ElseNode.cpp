#include "ElseNode.h"
#include <iostream>

namespace CHTL {

ElseNode::ElseNode(std::vector<std::unique_ptr<StylePropertyNode>> body)
    : body(std::move(body)) {}

void ElseNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "Else (" << std::endl;
    for (const auto& node : body) {
        node->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << ")" << std::endl;
}

} // namespace CHTL