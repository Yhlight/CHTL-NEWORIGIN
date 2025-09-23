#pragma once

#include "../CHTLNode/ElementNode.h"
#include "../CHTLExpr/BaseExprNode.h"
#include "../CHTLExpr/LiteralNode.h" // For ParsedValue
#include <memory>
#include <map>

// Forward declare the internal struct
struct EvaluatedValue;

#include "../CHTLContext/CHTLContext.h"

// ...

class StyleEvaluator {
public:
    void evaluate(const std::shared_ptr<ElementNode>& root);
    void evaluateGlobalRules(CHTLContext& context);

private:
    void evaluateNode(const std::shared_ptr<ElementNode>& node);
    EvaluatedValue evaluateExpression(const std::shared_ptr<BaseExprNode>& expr, const std::map<std::string, EvaluatedValue>& context);
};
