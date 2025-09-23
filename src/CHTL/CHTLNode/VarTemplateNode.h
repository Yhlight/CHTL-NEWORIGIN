#pragma once

#include "BaseNode.h"
#include <string>
#include <map>

// Represents a [Template] @Var definition in the AST.
// It stores a map of variable names to their string values.
class VarTemplateNode : public BaseNode {
public:
    VarTemplateNode() = default;
    NodeType getType() const override { return NodeType::VarTemplate; }

    bool isCustom = false;

    // The names of any parent templates this template inherits from.
    std::vector<std::string> parentNames;

    std::map<std::string, std::string> variables;
};
