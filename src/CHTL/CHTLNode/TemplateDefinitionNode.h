#pragma once

#include "BaseNode.h"
#include <string>

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(const std::string& category, const std::string& type, const std::string& name)
        : category(category), templateType(type), templateName(name) {}

    NodeType getType() const override { return NodeType::TemplateDefinition; }

    const std::string& getCategory() const { return category; }
    const std::string& getTemplateType() const { return templateType; }
    const std::string& getTemplateName() const { return templateName; }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<TemplateDefinitionNode>(category, templateType, templateName);
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

private:
    std::string category; // e.g., "[Template]", "[Custom]"
    std::string templateType; // e.g., "Style"
    std::string templateName;
};