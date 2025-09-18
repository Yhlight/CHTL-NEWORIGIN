#pragma once

#include "CHTLNode/Expression/ExpressionNode.h"
#include "CHTLNode/Expression/LiteralExpression.h"
#include "CHTLNode/Expression/BinaryOpExpression.h"
#include <string>

namespace CHTL {

// The ExpressionEvaluator uses the Visitor pattern to traverse an expression
// tree and compute a final string result.
class ExpressionEvaluator : public ExpressionVisitor {
public:
    std::string evaluate(ExpressionNode& root);

    void visit(LiteralExpression& node) override;
    void visit(BinaryOpExpression& node) override;

private:
    std::string m_result;
};

} // namespace CHTL
