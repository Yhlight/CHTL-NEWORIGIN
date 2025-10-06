#pragma once

#include "BaseNode.h"

namespace CHTL {
    class ExpressionNode : public BaseNode {
    public:
        ExpressionNode(NodeType type) : BaseNode(type) {}
        virtual ~ExpressionNode() = default;
    };
}