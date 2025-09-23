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

    // For Style customs
    std::map<std::string, std::shared_ptr<BaseExprNode>> styleProperties;
    std::vector<std::string> valuelessProperties;

    // For Element customs
    std::vector<std::shared_ptr<BaseNode>> children;

    // For Var customs
    std::map<std::string, std::string> variables;

    CustomDefinitionNode(const std::string& name, CustomType type)
        : BaseNode(NodeType::CustomDefinition), name(name), customType(type) {}
};
