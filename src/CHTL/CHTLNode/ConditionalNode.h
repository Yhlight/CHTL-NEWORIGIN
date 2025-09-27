#pragma once

#include "BaseNode.h"
#include "StyleValue.h"
#include <map>
#include <string>
#include <memory>

// Represents an 'if { ... }' block within an element.
// This node holds a condition and a set of style properties that are
// applied to the parent element if the condition evaluates to true.
class ConditionalNode : public BaseNode {
public:
    // The condition to be evaluated. This is expected to be a StyleValue
    // of type BOOL.
    std::unique_ptr<StyleValue> condition;

    // The style properties to apply if the condition is true.
    std::map<std::string, StyleValue> styles;

    ConditionalNode() = default;

    NodeType getType() const override {
        return NodeType::Conditional;
    }
};