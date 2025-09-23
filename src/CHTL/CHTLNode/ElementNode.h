#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "StyleValue.h"

// Represents an element node in the AST, e.g., html, body, div.
class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName);

    NodeType getType() const override;

    // The tag name of the element, e.g., "div".
    std::string tagName;

    // A map to store the element's attributes, e.g., {"id", "main"}.
    std::map<std::string, std::string> attributes;

    // A map to store parsed inline style properties for easy lookup.
    std::map<std::string, StyleValue> inlineStyles;

    // A vector of child nodes. Using unique_ptr for memory management.
    std::vector<std::unique_ptr<BaseNode>> children;
};
