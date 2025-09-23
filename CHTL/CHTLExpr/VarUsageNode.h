#pragma once

#include "BaseExprNode.h" // This was missing
#include <string>
#include <memory>

// Represents a variable usage, e.g., `ThemeColor(tableColor)` or
// with specialization: `ThemeColor(tableColor = "blue")`
struct VarUsageNode : public BaseExprNode {
    std::string groupName;
    std::string varName;
    std::shared_ptr<BaseExprNode> overrideExpr; // Null if not specialized

    VarUsageNode(const std::string& group, const std::string& var)
        : groupName(group), varName(var), overrideExpr(nullptr) {}

    ExprNodeType getType() const override { return ExprNodeType::VarUsage; }
};
