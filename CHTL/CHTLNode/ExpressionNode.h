#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "BaseNode.h"

namespace CHTL {

enum class ExpressionType {
    LITERAL,
    BINARY_OP
};

class ExpressionNode : public BaseNode {
public:
    explicit ExpressionNode(NodeType type) : BaseNode(type) {}
    virtual ~ExpressionNode() = default;
    virtual ExpressionType getExpressionType() const = 0;
};

} // namespace CHTL

#endif // EXPRESSION_NODE_H