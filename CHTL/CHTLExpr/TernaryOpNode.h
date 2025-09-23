#pragma once

#include "BaseExprNode.h"
#include <memory>

struct TernaryOpNode : public BaseExprNode {
    std::shared_ptr<BaseExprNode> condition;
    std::shared_ptr<BaseExprNode> trueExpr;
    std::shared_ptr<BaseExprNode> falseExpr;

    TernaryOpNode(
        std::shared_ptr<BaseExprNode> condition,
        std::shared_ptr<BaseExprNode> trueExpr,
        std::shared_ptr<BaseExprNode> falseExpr
    ) : condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) {}

    ExprNodeType getType() const override { return ExprNodeType::TernaryOp; }
};
