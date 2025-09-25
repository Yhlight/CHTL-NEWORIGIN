#pragma once

#include "BaseNode.h"

// Represents a {{&}} reference within a script block.
class AmpersandSelectorNode : public BaseNode {
public:
    AmpersandSelectorNode() = default;

    NodeType getType() const override {
        return NodeType::AmpersandSelector;
    }
};