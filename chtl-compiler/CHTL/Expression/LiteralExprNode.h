#pragma once

#include "ExprNode.h"

class LiteralExprNode : public ExprNode {
public:
    LiteralExprNode(const std::string& value) : value(value) {}

    std::string evaluate(const std::map<std::string, std::string>& context) const override {
        // A literal evaluates to itself.
        // For now, assume string literals have their quotes stripped by the parser.
        return value;
    }

private:
    std::string value;
};