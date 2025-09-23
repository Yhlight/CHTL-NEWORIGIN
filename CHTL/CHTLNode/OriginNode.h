#pragma once

#include "BaseNode.h"
#include <string>

// Represents a block of raw, unprocessed code
struct OriginNode : public BaseNode {
    std::string type; // e.g., "Html", "Style", "JavaScript"
    std::string rawContent;

    OriginNode(const std::string& type, const std::string& content)
        : BaseNode(NodeType::Origin), type(type), rawContent(content) {}
};
