#pragma once

#include <memory>
#include <string>

namespace CHTL {

// Forward declarations for expression nodes and the visitor
class LiteralExpression;
class BinaryOpExpression;
class ExpressionVisitor;

// Base class for all nodes in an expression tree.
class ExpressionNode {
public:
    virtual ~ExpressionNode() = default;
    // The accept method for the expression visitor pattern.
    // We will need to return a value from the visitor, so it's not a const method.
    // For now, let's keep it simple. A real implementation might return a variant type.
    virtual void accept(ExpressionVisitor& visitor) = 0;
    virtual std::unique_ptr<ExpressionNode> clone() const = 0;
};


// The visitor interface for the expression tree.
// This will be implemented by the ExpressionEvaluator.
class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual void visit(LiteralExpression& node) = 0;
    virtual void visit(BinaryOpExpression& node) = 0;
};

} // namespace CHTL
