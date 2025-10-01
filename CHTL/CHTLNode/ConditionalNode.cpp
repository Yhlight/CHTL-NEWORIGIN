#include "ConditionalNode.h"
#include <iostream>

namespace CHTL {

ConditionalNode::ConditionalNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body)
    : condition(std::move(condition)),
      body(std::move(body)),
      else_branch(nullptr) {}

void ConditionalNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "if (";
    if (condition) {
        // Assuming ExpressionNode has a print method or similar
        // condition->print(0);
    }
    std::cout << ")" << std::endl;

    for (const auto& prop : body) {
        prop->print(indent + 1);
    }

    if (else_branch) {
        for (int i = 0; i < indent; ++i) {
            std::cout << "  ";
        }
        std::cout << "else ";
        else_branch->print(indent);
    }
}

} // namespace CHTL