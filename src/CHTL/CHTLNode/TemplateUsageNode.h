#pragma once

#include "BaseNode.h"
#include <string>

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(const std::string& type, const std::string& name)
        : templateType(type), templateName(name) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }

    const std::string& getTemplateType() const { return templateType; }
    const std::string& getTemplateName() const { return templateName; }

private:
    std::string templateType; // e.g., "Style"
    std::string templateName;
};