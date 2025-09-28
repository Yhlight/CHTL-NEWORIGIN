#pragma once

#include "CHTLJSNode.h"
#include <string>

// This node represents a reserved placeholder for a block of pure JavaScript code.
// It allows the CHTL JS parser to correctly handle JS code embedded within
// CHTL JS constructs.
class ReservedPlaceholderNode : public CHTLJSNode {
public:
    explicit ReservedPlaceholderNode(const std::string& placeholder)
        : placeholder(placeholder) {}

    // The placeholder string, which will be replaced by the actual JS code
    // during the code generation phase.
    std::string placeholder;
    CHTLJSNodeType getType() const override { return CHTLJSNodeType::ReservedPlaceholder; }
};