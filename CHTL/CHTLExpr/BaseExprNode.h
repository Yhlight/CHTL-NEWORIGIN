#pragma once

#include <string>
#include <memory>

enum class ExprNodeType {
    Base,
    Literal,
    BinaryOp,
    TernaryOp,
    PropertyAccess // For accessing properties like 'width'
};

struct BaseExprNode {
    virtual ~BaseExprNode() = default;
    virtual ExprNodeType getType() const { return ExprNodeType::Base; }
};
