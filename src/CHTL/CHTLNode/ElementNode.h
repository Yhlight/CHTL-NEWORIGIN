#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "StyleValue.h"
#include "Constraint.h"

// Represents a block of styles that are applied only if a condition is met.
struct ConditionalStyleBlock {
    // The condition to evaluate. Can be empty for an 'else' block.
    StyleValue condition;
    // The styles to apply if the condition is true.
    std::map<std::string, StyleValue> styles;
};

// Represents an element node in the AST, e.g., html, body, div.
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName);

    NodeType getType() const override;

    // The tag name of the element, e.g., "div".
    std::string tagName;

    // A map to store the element's attributes, e.g., {"id", "main"}.
    std::map<std::string, StyleValue> attributes;

    // A map to store parsed inline style properties for easy lookup.
    std::map<std::string, StyleValue> inlineStyles;

    // A vector to hold a chain of if/else-if/else style blocks.
    std::vector<ConditionalStyleBlock> conditionalStyles;

    // A vector of child nodes. Using unique_ptr for memory management.
    std::vector<std::unique_ptr<BaseNode>> children;

    // A list of constraints defining which child types are not allowed.
    std::vector<Constraint> constraints;
};
