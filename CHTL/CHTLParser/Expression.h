#pragma once

#include "../CHTLLexer/Token.h"
#include <memory>
#include <string>

// Forward declarations
struct Binary;
struct Literal;
struct Grouping;

// Visitor for expressions
struct ExprVisitor {
    virtual ~ExprVisitor() = default;
    virtual double visit(Binary& expr) = 0;
    virtual double visit(Literal& expr) = 0;
    virtual double visit(Grouping& expr) = 0;
};

// Base class for expressions
struct Expr {
    virtual ~Expr() = default;
    virtual double accept(ExprVisitor& visitor) = 0;
};

struct Binary : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    double accept(ExprVisitor& visitor) override { return visitor.visit(*this); }
};

struct Literal : Expr {
    Token value;
    Literal(Token value) : value(value) {}
    double accept(ExprVisitor& visitor) override { return visitor.visit(*this); }
};

struct Grouping : Expr {
    std::unique_ptr<Expr> expression;
    Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}
    double accept(ExprVisitor& visitor) override { return visitor.visit(*this); }
};
