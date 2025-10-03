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
        auto node = std::make_unique<TemplateUsageNode>(templateType, templateName);
        for (const auto& child : getChildren()) {
            node->addChild(child->clone());
        }
        return node;
    }

private:
    std::string templateType; // e.g., "Style"
    std::string templateName;
};