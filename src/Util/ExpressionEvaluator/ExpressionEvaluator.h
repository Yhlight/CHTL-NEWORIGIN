#pragma once

#include <string>
#include <memory>

class ExpressionNode;
class BaseNode;

class ExpressionEvaluator {
public:
    std::string evaluate(const ExpressionNode* node, const BaseNode* astRoot, const BaseNode* contextNode);
};