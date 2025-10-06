#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {
    class VariableExpressionNode : public ExpressionNode {
    public:
        std::string name;

        VariableExpressionNode(const std::string& name)
            : ExpressionNode(NodeType::NODE_EXPRESSION_VARIABLE), name(name) {}
    };
}