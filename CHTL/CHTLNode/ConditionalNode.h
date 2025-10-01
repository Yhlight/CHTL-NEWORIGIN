#ifndef CHTL_CONDITIONALNODE_H
#define CHTL_CONDITIONALNODE_H

#include "ExpressionNode.h"
#include <memory>

namespace CHTL {

class ConditionalNode : public ExpressionNode {
public:
    ConditionalNode(std::unique_ptr<ExpressionNode> condition,
                    std::unique_ptr<ExpressionNode> trueExpression,
                    std::unique_ptr<ExpressionNode> falseExpression);

    ExpressionType getType() const override;
    void print(int indent = 0) const override;

    const ExpressionNode* getCondition() const;
    const ExpressionNode* getTrueExpression() const;
    const ExpressionNode* getFalseExpression() const;

private:
    std::unique_ptr<ExpressionNode> condition;
    std::unique_ptr<ExpressionNode> trueExpression;
    std::unique_ptr<ExpressionNode> falseExpression;
};

} // namespace CHTL

#endif // CHTL_CONDITIONALNODE_H