#pragma once

#include <string>
#include <memory>

class ExpressionNode;

class ExpressionEvaluator {
public:
    std::string evaluate(const ExpressionNode* node);
};