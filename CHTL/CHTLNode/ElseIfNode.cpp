#include "ElseIfNode.h"
#include <iostream>

namespace CHTL {

ElseIfNode::ElseIfNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body)
    : condition(std::move(condition)), body(std::move(body)) {}

void ElseIfNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "ElseIf (" << std::endl;
    condition->print(indent + 1);
    for (const auto& node : body) {
        node->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << ")" << std::endl;
}

} // namespace CHTL