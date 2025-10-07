#pragma once

#include "ExprNode.h"

class ConditionalExprNode : public ExprNode {
public:
    ConditionalExprNode(ExprNodePtr condition, ExprNodePtr true_branch, ExprNodePtr false_branch)
        : condition(condition), true_branch(true_branch), false_branch(false_branch) {}

    std::string evaluate(const std::map<std::string, std::string>& context) const override {
        std::string cond_result = condition->evaluate(context);
        // Evaluate to true if the result is non-zero or a non-empty string.
        // This is a simplification; a real implementation would have a proper type system.
        bool is_true = (cond_result != "0" && !cond_result.empty());

        if (is_true) {
            return true_branch->evaluate(context);
        } else {
            return false_branch->evaluate(context);
        }
    }

private:
    ExprNodePtr condition;
    ExprNodePtr true_branch;
    ExprNodePtr false_branch;
};