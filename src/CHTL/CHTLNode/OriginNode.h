#pragma once

#include "BaseNode.h"
#include <string>

// Represents an [Origin] block in the AST.
// This node stores a chunk of raw, unprocessed text that the
// generator will print directly into the output.
class OriginNode : public BaseNode {
public:
    explicit OriginNode(const std::string& content) : content(content) {}
    NodeType getType() const override { return NodeType::Origin; }

    std::string content;
};
