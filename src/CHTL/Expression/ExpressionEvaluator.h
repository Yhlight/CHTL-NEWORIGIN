#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "ExpressionNode.h"
#include <memory>

namespace CHTL {

// Using NumberNode to represent the result of an evaluation
using EvaluationResult = NumberNode;

class ExpressionEvaluator {
public:
    static EvaluationResult evaluate(const IExpressionNode& node);

private:
    static EvaluationResult visit(const NumberNode& node);
    static EvaluationResult visit(const BinaryOpNode& node);
};

} // namespace CHTL

#endif // EXPRESSION_EVALUATOR_H
