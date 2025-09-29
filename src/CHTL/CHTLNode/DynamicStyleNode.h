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

    // Cloning a dynamic style node now requires cloning the underlying expression AST.
    // This functionality will need to be added to the expression nodes themselves.
    virtual std::unique_ptr<StyleValue> clone() const override {
        // Placeholder: A full implementation requires a clone() method on ExpressionBaseNode.
        return nullptr;
    }
};