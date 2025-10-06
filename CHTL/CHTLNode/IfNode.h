#pragma once

#include "BaseNode.h"
#include "PropertyNode.h"
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

        IfNode() : BaseNode(NodeType::NODE_IF) {}
    };
}