#ifndef BINARY_OP_NODE_H
#define BINARY_OP_NODE_H

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"
#include <memory>

namespace CHTL {

class BinaryOpNode : public ExpressionNode {
public:
    BinaryOpNode(std::unique_ptr<ExpressionNode> left, const Token& op, std::unique_ptr<ExpressionNode> right)
        : ExpressionNode(NodeType::BINARY_OP), left(std::move(left)), op(op), right(std::move(right)) {}

    void print(int indent = 0) const override;
    ExpressionType getExpressionType() const override { return ExpressionType::BINARY_OP; }
    std::unique_ptr<BaseNode> clone() const override;

    const ExpressionNode* getLeft() const;
    const Token& getOperator() const;
    const ExpressionNode* getRight() const;

private:
    std::unique_ptr<ExpressionNode> left;
    Token op;
    std::unique_ptr<ExpressionNode> right;
};

} // namespace CHTL

#endif // BINARY_OP_NODE_H