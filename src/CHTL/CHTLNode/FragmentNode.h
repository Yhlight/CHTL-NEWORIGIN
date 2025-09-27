#pragma once

#include "BaseNode.h"
#include <vector>
#include <memory>

// Represents a "fragment" of the AST. This is an invisible node
// that holds a list of children. It's used to represent the result
// of expanding an @Element template, which can contain multiple
// sibling nodes. The generator will render this node's children
// directly without any wrapping tag.
class FragmentNode : public BaseNode {
public:
    FragmentNode() = default;
    NodeType getType() const override { return NodeType::Fragment; }

    std::vector<std::unique_ptr<BaseNode>> children;
};
