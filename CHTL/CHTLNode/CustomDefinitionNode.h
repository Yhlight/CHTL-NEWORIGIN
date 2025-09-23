#pragma once

#include "BaseNode.h"
#include "../CHTLExpr/BaseExprNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

enum class CustomType {
    Style,
    Element,
    Var
};

struct CustomDefinitionNode : public BaseNode {
    std::string name;
    CustomType customType;

    // Content for Style custom definitions
    std::map<std::string, std::shared_ptr<BaseExprNode>> styleProperties;

    // Content for Element custom definitions
    std::vector<std::shared_ptr<BaseNode>> children;

    // Content for Var custom definitions
    std::map<std::string, std::string> variables;

    CustomDefinitionNode(const std::string& name, CustomType type)
        : name(name), customType(type) {}

    NodeType getType() const override { return NodeType::CustomDefinition; }
};
