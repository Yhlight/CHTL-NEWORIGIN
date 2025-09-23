#pragma once

#include "BaseNode.h"
#include "ElementNode.h"
#include <string>
#include <memory>

// Represents modifying an existing element in a usage block.
// e.g., div[1] { style { ... } }
struct ModifyElementNode : public BaseNode {
    std::string targetSelector;
    // We can reuse ElementNode to hold the modifications (attributes, styles, etc.).
    std::shared_ptr<ElementNode> modifications;

    ModifyElementNode(const std::string& selector)
        : targetSelector(selector) {
        // Use a temporary tag name for the container
        modifications = std::make_shared<ElementNode>("__MODIFICATIONS__");
    }

    NodeType getType() const override { return NodeType::ModifyElement; }
};
