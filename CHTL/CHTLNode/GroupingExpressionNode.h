#pragma once

#include "ExpressionNode.h"
#include <memory>

namespace CHTL {
    class GroupingExpressionNode : public ExpressionNode {
    public:
        std::shared_ptr<ExpressionNode> expression;

        GroupingExpressionNode(std::shared_ptr<ExpressionNode> expression)
            : ExpressionNode(NodeType::NODE_EXPRESSION_GROUPING), expression(expression) {}
    };
}