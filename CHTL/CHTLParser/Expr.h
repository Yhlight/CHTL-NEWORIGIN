#pragma once

#include "../CHTLLexer/Token.h"
#include <memory>
#include <any>
#include <vector>

namespace CHTL {

// Forward declarations for all expression node classes
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct GroupingExpr;
struct TernaryExpr; // For conditional expressions like condition ? true_val : false_val

// The Visitor interface for our expression AST.
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual std::any visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual std::any visitUnaryExpr(const UnaryExpr& expr) = 0;
    virtual std::any visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual std::any visitGroupingExpr(const GroupingExpr& expr) = 0;
    virtual std::any visitTernaryExpr(const TernaryExpr& expr) = 0;
};

// The base class for all expression nodes.
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::any accept(ExprVisitor& visitor) const = 0;
};

// An expression with a left-hand side, an operator, and a right-hand side.
struct BinaryExpr : Expr {
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    const std::unique_ptr<Expr> left;
    const Token op;
    const std::unique_ptr<Expr> right;
};

// An expression with a single operator and an operand.
struct UnaryExpr : Expr {
    UnaryExpr(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    const Token op;
    const std::unique_ptr<Expr> right;
};

// A literal value like a number, string, or unquoted literal.
struct LiteralExpr : Expr {
    LiteralExpr(Token value) : value(std::move(value)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    const Token value;
};

// An expression enclosed in parentheses.
struct GroupingExpr : Expr {
    GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    const std::unique_ptr<Expr> expression;
};

// A conditional (ternary) expression.
struct TernaryExpr : Expr {
    TernaryExpr(std::unique_ptr<Expr> condition, std::unique_ptr<Expr> thenBranch, std::unique_ptr<Expr> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(ExprVisitor& visitor) const override {
        return visitor.visitTernaryExpr(*this);
    }

    const std::unique_ptr<Expr> condition;
    const std::unique_ptr<Expr> thenBranch;
    const std::unique_ptr<Expr> elseBranch;
};

} // namespace CHTL