#pragma once

#include "BaseNode.h"
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

    // The styles are stored as key-value pairs. The value is a raw
    // string because it might contain an expression that needs to be
    // evaluated later in the context where the template is used.
    std::map<std::string, std::string> styles;

    // For custom templates, stores properties defined without a value.
    std::vector<std::string> valuelessProperties;

    // Stores the names of parent templates for inheritance.
    std::vector<std::string> parentNames;
};
