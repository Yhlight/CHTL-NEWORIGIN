#include "ConditionalNode.h"
#include <iostream>

namespace CHTL {

void IfNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "If (" << std::endl;
    condition->print(indent + 2);
    std::cout << std::string(indent, ' ') << ") {" << std::endl;
    for (const auto& prop : body) {
        prop->print(indent + 2);
    }
    std::cout << std::string(indent, ' ') << "}" << std::endl;
    if (else_branch) {
        else_branch->print(indent);
    }
}

void ElseIfNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Else If (" << std::endl;
    condition->print(indent + 2);
    std::cout << std::string(indent, ' ') << ") {" << std::endl;
    for (const auto& prop : body) {
        prop->print(indent + 2);
    }
    std::cout << std::string(indent, ' ') << "}" << std::endl;
    if (next_else_branch) {
        next_else_branch->print(indent);
    }
}

void ElseNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Else {" << std::endl;
    for (const auto& prop : body) {
        prop->print(indent + 2);
    }
    std::cout << std::string(indent, ' ') << "}" << std::endl;
}

} // namespace CHTL