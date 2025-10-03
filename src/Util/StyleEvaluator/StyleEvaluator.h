#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../ExpressionEvaluator/ExpressionEvaluator.h"

class StyleEvaluator {
public:
    void evaluate(BaseNode* root);

private:
    void traverse(BaseNode* node);
    ExpressionEvaluator evaluator;
    BaseNode* astRoot = nullptr;
};