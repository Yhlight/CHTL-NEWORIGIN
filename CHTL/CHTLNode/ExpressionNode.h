#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "BaseNode.h"

namespace CHTL {

enum class ExpressionType {
    LITERAL,
    BINARY_OP,
    PROPERTY_ACCESS,
    CONDITIONAL
};

class ExpressionNode : public BaseNode {
public:
    virtual ~ExpressionNode() = default;
    virtual ExpressionType getType() const = 0;
};

} // namespace CHTL

#endif // EXPRESSION_NODE_H