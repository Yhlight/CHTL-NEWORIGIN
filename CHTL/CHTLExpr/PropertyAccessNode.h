#pragma once

#include "BaseExprNode.h"
#include <string>

// Represents an access to another property, e.g., 'width' in an expression
struct PropertyAccessNode : public BaseExprNode {
    std::string propertyName;

    explicit PropertyAccessNode(const std::string& name) : propertyName(name) {}

    ExprNodeType getType() const override { return ExprNodeType::PropertyAccess; }
};
