#pragma once

#include "BaseNode.h"
#include "StyleValue.h" // Include StyleValue definition
#include <string>
#include <vector>
#include <map>

// Represents a [Template] @Style definition in the AST.
// It's not a node that gets rendered directly, but a definition
// stored by the TemplateManager.
class StyleTemplateNode : public BaseNode {
public:
    StyleTemplateNode() = default;
    NodeType getType() const override { return NodeType::StyleTemplate; }

    bool isCustom = false;

    // The styles are stored as key-value pairs, using unique_ptr to support polymorphism.
    std::map<std::string, std::unique_ptr<StyleValue>> styles;

    // For custom templates, stores properties defined without a value.
    std::vector<std::string> valuelessProperties;

    // Stores the names of parent templates for inheritance.
    std::vector<std::string> parentNames;

    // For custom style specialization, stores properties to be deleted.
    std::vector<std::string> deletedProperties;
};
