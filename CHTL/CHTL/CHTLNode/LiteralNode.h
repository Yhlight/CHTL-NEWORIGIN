#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class LiteralNode : public ExpressionNode {
public:
    Token value;

    LiteralNode(const Token& val) : value(val) {}

    Value accept(ExpressionVisitor& visitor) override {
        return visitor.visit(*this);
    }
};

} // namespace CHTL
