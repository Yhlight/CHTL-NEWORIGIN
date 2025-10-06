#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
    enum class IfType {
        IF,
        ELSE_IF,
        ELSE
    };

    class IfNode : public BaseNode {
    public:
        IfType if_type;
        std::string condition;
        std::shared_ptr<IfNode> next_if = nullptr;

        IfNode() : BaseNode(NodeType::NODE_IF) {}
    };
}