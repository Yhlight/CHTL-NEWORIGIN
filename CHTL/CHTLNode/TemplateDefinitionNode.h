#pragma once

#include "BaseNode.h"
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
    std::map<std::string, std::string> styleProperties;

    // Content for Element templates
    std::vector<std::shared_ptr<BaseNode>> children;

    // We can add content for Var templates later

    TemplateDefinitionNode(const std::string& name, TemplateType type)
        : name(name), templateType(type) {}

    NodeType getType() const override { return NodeType::TemplateDefinition; }
};
