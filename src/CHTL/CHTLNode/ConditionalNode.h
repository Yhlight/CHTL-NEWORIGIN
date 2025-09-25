#pragma once

#include "ElementNode.h"
#include "StyleValue.h"

// Represents an 'if' block for conditional rendering.
// It holds the condition to be evaluated and the child nodes to be rendered
// if the condition is true.
class ConditionalNode : public BaseNode {
public:
    // The expression that determines whether the children are rendered.
    StyleValue condition;

    // The nodes to be rendered if the condition is true.
    std::vector<std::unique_ptr<BaseNode>> children;

    NodeType getType() const override { return NodeType::Conditional; }
};