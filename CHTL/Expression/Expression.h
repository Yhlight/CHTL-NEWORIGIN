#pragma once

#include <memory>
#include "CHTL/Lexer/Token.h"
#include "CHTL/Evaluator/Value.h"

// Forward declare all expression types and the visitor
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class ReferenceExpr;
class ConditionalExpr;
class LogicalExpr;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual Value visit(const BinaryExpr& expr) = 0;
    virtual Value visit(const UnaryExpr& expr) = 0;
    virtual Value visit(const LiteralExpr& expr) = 0;
    virtual Value visit(const ReferenceExpr& expr) = 0;
    virtual Value visit(const ConditionalExpr& expr) = 0;
    virtual Value visit(const LogicalExpr& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;
    virtual Value accept(ExprVisitor& visitor) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;
