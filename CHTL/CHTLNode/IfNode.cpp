#include "IfNode.h"
#include <iostream>

namespace CHTL {

IfNode::IfNode(std::unique_ptr<ExpressionNode> condition, std::vector<std::unique_ptr<StylePropertyNode>> body)
    : m_condition(std::move(condition)), m_body(std::move(body)), m_else_branch(nullptr) {}

void IfNode::setElseBranch(std::unique_ptr<IfNode> else_branch) {
    m_else_branch = std::move(else_branch);
}

void IfNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "If (";
    if (m_condition) {
        m_condition->print();
    } else {
        std::cout << "else";
    }
    std::cout << ") {" << std::endl;
    for (const auto& prop : m_body) {
        prop->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "}" << std::endl;
    if (m_else_branch) {
        m_else_branch->print(indent);
    }
}

const ExpressionNode* IfNode::getCondition() const {
    return m_condition.get();
}

const std::vector<std::unique_ptr<StylePropertyNode>>& IfNode::getBody() const {
    return m_body;
}

const IfNode* IfNode::getElseBranch() const {
    return m_else_branch.get();
}

} // namespace CHTL