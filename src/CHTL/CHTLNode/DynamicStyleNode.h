#pragma once

#include "StyleValue.h"
#include "../ExpressionNode/ExpressionBaseNode.h"
#include <string>
#include <memory>

class DynamicStyleNode : public StyleValue {
public:
    std::unique_ptr<ExpressionBaseNode> expressionAst;

    explicit DynamicStyleNode(std::unique_ptr<ExpressionBaseNode> ast)
        : expressionAst(std::move(ast)) {}

    // Note: The toString method is now less useful for direct output,
    // as the value is determined by evaluation, not a simple string.
    // It could be updated to serialize the expression AST for debugging.
    virtual std::string toString() const override {
        return "DynamicStyleNode(AST)";
    }

    virtual StyleValueType getType() const override {
        return StyleValueType::Dynamic;
    }

    virtual std::unique_ptr<StyleValue> clone() const override {
        return std::make_unique<DynamicStyleNode>(expressionAst->clone());
    }
};