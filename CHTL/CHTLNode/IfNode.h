#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

enum class IfType {
    IF,
    ELSE_IF,
    ELSE
};

class IfNode : public BaseNode {
public:
    IfType if_type;
    std::shared_ptr<BaseNode> condition;

    IfNode() : BaseNode(NodeType::NODE_IF) {}

    std::string ToString(const std::string& indent = "", bool isLast = true) const override {
        std::string type_str;
        switch (if_type) {
            case IfType::IF: type_str = "If"; break;
            case IfType::ELSE_IF: type_str = "Else If"; break;
            case IfType::ELSE: type_str = "Else"; break;
        }
        std::string result = indent + (isLast ? "`-- " : "|-- ") + type_str + "Node:\n";
        if (condition) {
            result += condition->ToString(indent + (isLast ? "    " : "|   "), false);
        }

        const auto& children = getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            result += children[i]->ToString(indent + (isLast ? "    " : "|   "), i == children.size() - 1);
        }
        return result;
    }
};

}