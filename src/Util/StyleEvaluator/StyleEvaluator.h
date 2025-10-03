#pragma once

#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../ExpressionEvaluator/ExpressionEvaluator.h"

class StyleEvaluator {
public:
    void evaluate(BaseNode* root);

private:
    void traverse(BaseNode* node, ElementNode* containingElement);
    void processElementNode(ElementNode* elementNode);
    ExpressionEvaluator evaluator;
    BaseNode* astRoot = nullptr;
};