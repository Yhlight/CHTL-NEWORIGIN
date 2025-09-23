#pragma once

#include "BaseNode.h"
#include "../CHTLExpr/BaseExprNode.h" // Correct placement
#include <string>
#include <vector>
#include <map>
#include <memory>

enum class TemplateType {
    Style,
    Element,
    Var
};

struct TemplateDefinitionNode : public BaseNode {
    std::string name;
    TemplateType templateType;

    // Content for Style templates
    std::map<std::string, std::shared_ptr<BaseExprNode>> styleProperties;

    // Content for Element templates
    std::vector<std::shared_ptr<BaseNode>> children;

    // Content for Var templates
    std::map<std::string, std::string> variables;

    TemplateDefinitionNode(const std::string& name, TemplateType type)
        : name(name), templateType(type) {}

    NodeType getType() const override { return NodeType::TemplateDefinition; }
};
