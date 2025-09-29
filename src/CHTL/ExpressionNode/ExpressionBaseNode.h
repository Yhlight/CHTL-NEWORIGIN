#pragma once

#include <string>
#include <memory>

// Enum to identify the type of an expression AST node.
enum class ExpressionNodeType {
    Literal,
    PropertyRef,
    BinaryOp,
    Conditional
};

// Base class for all nodes in an expression's Abstract Syntax Tree (AST).
class ExpressionBaseNode {
public:
    virtual ~ExpressionBaseNode() = default;
    virtual ExpressionNodeType getType() const = 0;
};