#pragma once

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For the TemplateType enum
#include <string>
#include <memory>

struct TemplateUsageNode : public BaseNode {
    std::string name;
    TemplateType templateType;

    TemplateUsageNode(const std::string& name, TemplateType type)
        : name(name), templateType(type) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }
};
