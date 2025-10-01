#include "IfNode.h"
#include <iostream>

namespace CHTL {

IfNode::IfNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body)
    : condition(std::move(condition)), body(std::move(body)) {}

void IfNode::addElseIf(std::unique_ptr<ElseIfNode> elseIfNode) {
    elseIfClauses.push_back(std::move(elseIfNode));
}

void IfNode::setElse(std::unique_ptr<ElseNode> elseNode) {
    elseClause = std::move(elseNode);
}

void IfNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "If (" << std::endl;
    condition->print(indent + 1);
    for (const auto& node : body) {
        node->print(indent + 1);
    }
    for (const auto& elseIfNode : elseIfClauses) {
        elseIfNode->print(indent);
    }
    if (elseClause) {
        elseClause->print(indent);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << ")" << std::endl;
}

} // namespace CHTL