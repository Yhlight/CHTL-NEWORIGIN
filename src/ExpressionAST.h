#ifndef CHTL_EXPRESSION_AST_H
#define CHTL_EXPRESSION_AST_H

#include <string>
#include <memory>

// Base class for all expression nodes
class ExprNode {
public:
    virtual ~ExprNode() = default;
    // We'll need an evaluation method later, but for now, the structure is enough.
    // virtual double evaluate() = 0;
};

// Represents a numeric literal with a unit
class NumberNode : public ExprNode {
public:
    double value;
    std::string unit;

    NumberNode(double value, std::string unit)
        : value(value), unit(std::move(unit)) {}
};

// Represents a binary operation
class BinaryOpNode : public ExprNode {
public:
    char op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;

    BinaryOpNode(char op, std::unique_ptr<ExprNode> left, std::unique_ptr<ExprNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};


#endif //CHTL_EXPRESSION_AST_H
