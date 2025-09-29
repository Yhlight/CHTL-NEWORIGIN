#pragma once

#include "StyleValue.h"
#include <string>

class DynamicStyleNode : public StyleValue {
private:
    std::string expression;

public:
    explicit DynamicStyleNode(const std::string& expr) : expression(expr) {}

    const std::string& getExpression() const {
        return expression;
    }

    virtual std::string toString() const override {
        return "DynamicStyleNode(Expression: " + expression + ")";
    }

    virtual StyleValueType getType() const override {
        return StyleValueType::Dynamic;
    }
};