#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

// Represents a [Template] @Element definition in the AST.
// It holds a snippet of the AST (a list of child nodes) that can be
// instantiated later when the template is used.
class ElementTemplateNode : public BaseNode {
public:
    ElementTemplateNode() = default;
    NodeType getType() const override { return NodeType::ElementTemplate; }

    // The child nodes that make up the template's content.
    std::vector<std::unique_ptr<BaseNode>> children;
};
