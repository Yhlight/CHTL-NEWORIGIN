#pragma once

#include "BaseNode.h"
#include <string>

// Represents an [Origin] block in the AST.
// This node stores a chunk of raw, unprocessed text that the
// generator will print directly into the output.
class OriginNode : public BaseNode {
public:
    // Constructor for an anonymous origin block
    explicit OriginNode(std::string type, std::string content)
        : type(std::move(type)), content(std::move(content)), name("") {}

    // Constructor for a named origin block
    OriginNode(std::string type, std::string content, std::string name)
        : type(std::move(type)), content(std::move(content)), name(std::move(name)) {}

    NodeType getType() const override { return NodeType::Origin; }

    std::string type;    // e.g., "Html", "Style", "JavaScript"
    std::string content; // The raw content inside the block
    std::string name;    // The optional name for reusing the block
};
