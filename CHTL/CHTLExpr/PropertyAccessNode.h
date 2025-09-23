#pragma once

#include "BaseExprNode.h"
#include <string>

// Represents an access to another property, e.g., 'width' or '.box.width'
struct PropertyAccessNode : public BaseExprNode {
    std::string selector;
    std::string propertyName;

    explicit PropertyAccessNode(const std::string& sel, const std::string& name) : selector(sel), propertyName(name) {}

    ExprNodeType getType() const override { return ExprNodeType::PropertyAccess; }
};
