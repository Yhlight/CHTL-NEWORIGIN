#pragma once

#include "BaseNode.h"
#include <string>

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(const std::string& type, const std::string& name)
        : templateType(type), templateName(name) {}

    NodeType getType() const override { return NodeType::TemplateDefinition; }

    const std::string& getTemplateType() const { return templateType; }
    const std::string& getTemplateName() const { return templateName; }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<TemplateDefinitionNode>(templateType, templateName);
        for (const auto& child : getChildren()) {
            node->addChild(child->clone());
        }
        return node;
    }

private:
    std::string templateType; // e.g., "Style"
    std::string templateName;
};