#pragma once

#include "BaseExprNode.h"
#include <string>

// Represents an identifier in an expression, which could be a property
// reference or a literal string value (e.g., 'red', 'bold').
struct IdentifierNode : public BaseExprNode {
    std::string name;

    explicit IdentifierNode(const std::string& name) : name(name) {}

    ExprNodeType getType() const override { return ExprNodeType::Identifier; }
    std::shared_ptr<BaseExprNode> clone() const override {
        return std::make_shared<IdentifierNode>(name);
    }
};
