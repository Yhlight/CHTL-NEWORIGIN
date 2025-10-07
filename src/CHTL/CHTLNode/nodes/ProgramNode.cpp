#include "ProgramNode.h"
#include "../BaseNode.h"
#include <sstream>

namespace CHTL {

ProgramNode::ProgramNode() : BaseNode(NodeType::Program) {}

void ProgramNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

String ProgramNode::toString(int indent) const {
    std::stringstream ss;
    ss << indentString(indent) << "Program\n";
    for (const auto& child : children_) {
        ss << child->toString(indent + 1) << "\n";
    }
    return ss.str();
}

SharedPtr<BaseNode> ProgramNode::clone() const {
    auto node = std::make_shared<ProgramNode>();
    for (const auto& child : children_) {
        node->addChild(child->clone());
    }
    return node;
}

} // namespace CHTL
