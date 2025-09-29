#pragma once

#include "CHTLJSBaseNode.h"
#include <string>

class EnhancedSelectorNode : public CHTLJSBaseNode {
private:
    std::string selector;
    std::string expression;

public:
    EnhancedSelectorNode(const std::string& sel, const std::string& expr)
        : selector(sel), expression(expr) {}

    const std::string& getSelector() const {
        return selector;
    }

    const std::string& getExpression() const {
        return expression;
    }

    virtual std::string toString() const override {
        return "EnhancedSelectorNode(Selector: " + selector + ", Expression: " + expression + ")";
    }
};