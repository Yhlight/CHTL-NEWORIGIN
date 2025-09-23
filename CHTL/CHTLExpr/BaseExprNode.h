#pragma once

#include <string>
#include <memory>

struct VarUsageNode; // Forward Declaration

enum class ExprNodeType {
    Base,
    Literal,
    BinaryOp,
    TernaryOp,
    PropertyAccess, // For accessing properties like '.box.width'
    Identifier,     // For ambiguous identifiers like 'red' or 'width'
    VarUsage
};

struct BaseExprNode {
    virtual ~BaseExprNode() = default;
    virtual ExprNodeType getType() const { return ExprNodeType::Base; }
    virtual std::shared_ptr<BaseExprNode> clone() const = 0;
};
