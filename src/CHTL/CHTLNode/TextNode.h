#pragma once

#include "BaseNode.h"
#include <string>

// Represents a text node in the AST.
class TextNode : public BaseNode {
public:
    explicit TextNode(const std::string& text);

    NodeType getType() const override;

    // The content of the text node.
    std::string text;
};
