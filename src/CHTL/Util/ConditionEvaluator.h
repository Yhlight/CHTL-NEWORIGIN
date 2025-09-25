#pragma once

#include "../CHTLNode/StyleValue.h"
#include "../CHTLNode/ElementNode.h"

class ConditionEvaluator {
public:
    static bool evaluate(const StyleValue& condition, const ElementNode* context);
};