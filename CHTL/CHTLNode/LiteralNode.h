#ifndef LITERAL_NODE_H
#define LITERAL_NODE_H

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

class LiteralNode : public ExpressionNode {
public:
    explicit LiteralNode(const Token& value)
        : ExpressionNode(NodeType::LITERAL), value(value) {}

    void print(int indent = 0) const override;
    ExpressionType getExpressionType() const override { return ExpressionType::LITERAL; }
    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<LiteralNode>(value);
    }
    const Token& getValue() const { return value; }

private:
    Token value;
};

} // namespace CHTL

#endif // LITERAL_NODE_H