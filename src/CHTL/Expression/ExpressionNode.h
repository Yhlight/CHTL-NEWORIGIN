#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include <string>
#include <memory>
#include "CHTL/CHTLLexer/Token.h"

namespace CHTL {

// Interface for all expression nodes
class IExpressionNode {
public:
    virtual ~IExpressionNode() = default;
};

// Represents a number and its unit, e.g., "100px"
class NumberNode : public IExpressionNode {
public:
    double value;
    std::string unit;

    NumberNode(double v = 0.0, const std::string& u = "") : value(v), unit(u) {}
};

// Represents a binary operation, e.g., A + B
class BinaryOpNode : public IExpressionNode {
public:
    std::unique_ptr<IExpressionNode> left;
    TokenType op;
    std::unique_ptr<IExpressionNode> right;
};

} // namespace CHTL

#endif // EXPRESSION_NODE_H
