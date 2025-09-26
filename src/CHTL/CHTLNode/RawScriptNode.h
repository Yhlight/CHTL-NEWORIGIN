#pragma once

#include "BaseNode.h"
#include <string>

// Represents a block of raw, non-CHTL-JS script content that appears
// between or around enhanced selectors.
class RawScriptNode : public BaseNode {
public:
    std::string content;

    explicit RawScriptNode(std::string content);

    NodeType getType() const override {
        return NodeType::RawScript;
    }
};