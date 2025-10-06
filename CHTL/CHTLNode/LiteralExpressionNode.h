#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {
    class LiteralExpressionNode : public ExpressionNode {
    public:
        std::string value;

        LiteralExpressionNode(const std::string& value)
            : ExpressionNode(NodeType::NODE_EXPRESSION_LITERAL), value(value) {}
    };
}