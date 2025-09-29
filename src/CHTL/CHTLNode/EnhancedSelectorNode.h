#pragma once

#include "BaseNode.h"
#include <string>

// Represents an enhanced CHTL JS selector, e.g., {{ .box }}
class EnhancedSelectorNode : public BaseNode {
public:
    std::string selector;

    explicit EnhancedSelectorNode(std::string selector);

    NodeType getType() const override {
        return NodeType::EnhancedSelector;
    }
};