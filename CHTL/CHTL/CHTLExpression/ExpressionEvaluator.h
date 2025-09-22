#pragma once

#include "../CHTLNode/ExpressionNode.h"
#include <string>

namespace CHTL {

class ExpressionEvaluator : public ExpressionVisitor {
public:
    Value evaluate(ExpressionNode* root);

    Value visit(BinaryOpNode& node) override;
    Value visit(LiteralNode& node) override;
};

} // namespace CHTL
