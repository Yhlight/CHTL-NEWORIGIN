#pragma once

#include "BaseNode.h"
#include <string>

// Represents a responsive value, e.g., $myVar$
class ResponsiveValueNode : public BaseNode {
public:
    std::string variableName;

    explicit ResponsiveValueNode(std::string variableName);

    NodeType getType() const override {
        return NodeType::ResponsiveValue;
    }
};