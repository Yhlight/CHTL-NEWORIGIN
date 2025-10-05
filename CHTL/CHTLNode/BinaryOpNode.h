#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class BinaryOpNode : public BaseNode {
public:
    std::string op;
    std::shared_ptr<BaseNode> left;
    std::shared_ptr<BaseNode> right;

    BinaryOpNode(const std::string& op, std::shared_ptr<BaseNode> left, std::shared_ptr<BaseNode> right)
        : BaseNode(NodeType::NODE_BINARY_OP), op(op), left(left), right(right) {
        // We cannot call shared_from_this() in the constructor, so we can't use addChild.
        // Instead, we manually manage the children and set their parent later.
        children.push_back(left);
        children.push_back(right);
    }

    std::string ToString(const std::string& indent = "", bool isLast = true) const override {
        std::string result = indent + (isLast ? "`-- " : "|-- ") + "BinaryOpNode: " + op + "\n";
        const auto& children_nodes = getChildren();
        for (size_t i = 0; i < children_nodes.size(); ++i) {
            result += children_nodes[i]->ToString(indent + (isLast ? "    " : "|   "), i == children_nodes.size() - 1);
        }
        return result;
    }
};

}