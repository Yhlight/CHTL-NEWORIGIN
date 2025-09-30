#ifndef LITERAL_NODE_H
#define LITERAL_NODE_H

#include "ExpressionNode.h"
#include "../CHTLLexer/Token.h"

namespace CHTL {

class LiteralNode : public ExpressionNode {
public:
    explicit LiteralNode(const Token& value);
    void print(int indent = 0) const override;
    ExpressionType getType() const override;
    const Token& getValue() const;

private:
    Token value;
};

} // namespace CHTL

#endif // LITERAL_NODE_H