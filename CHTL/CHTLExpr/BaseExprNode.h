#pragma once

#include <string>
#include <memory>

enum class ExprNodeType {
    Base,
    Literal,
    BinaryOp,
    TernaryOp,
    PropertyAccess, // For accessing properties like '.box.width'
    Identifier      // For ambiguous identifiers like 'red' or 'width'
};

struct BaseExprNode {
    virtual ~BaseExprNode() = default;
    virtual ExprNodeType getType() const { return ExprNodeType::Base; }
};
