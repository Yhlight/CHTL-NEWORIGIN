#pragma once

#include "ExpressionBaseNode.h"
#include <string>

// Represents a reference to another element's property, e.g., ".box.width".
class PropertyRefNode : public ExpressionBaseNode {
public:
    std::string selector;
    std::string propertyName;

    PropertyRefNode(const std::string& sel, const std::string& prop)
        : selector(sel), propertyName(prop) {}

    ExpressionNodeType getType() const override {
        return ExpressionNodeType::PropertyRef;
    }
};