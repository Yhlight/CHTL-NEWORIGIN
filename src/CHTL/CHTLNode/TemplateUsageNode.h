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

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<TemplateUsageNode>(templateType, templateName);
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

private:
    std::string templateType; // e.g., "Style"
    std::string templateName;
};