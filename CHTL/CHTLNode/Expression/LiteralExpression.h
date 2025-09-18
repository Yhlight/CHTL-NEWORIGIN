#pragma once

#include "ExpressionNode.h"
#include "../../CHTLLexer/Token.h"

namespace CHTL {

// Represents a literal value in an expression tree, e.g., "100px", "red", "'hello'".
class LiteralExpression : public ExpressionNode {
public:
    explicit LiteralExpression(Token token) : token(std::move(token)) {}

    void accept(ExpressionVisitor& visitor) override {
        visitor.visit(*this);
    }

    Token token;
};

} // namespace CHTL
