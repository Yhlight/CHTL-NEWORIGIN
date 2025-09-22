#pragma once

#include "StyleStatementNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum

class TemplateUsageNode : public StyleStatementNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name)
        : templateType(type), name(name) {}

    void accept(Visitor& visitor) override {
        // The generator will not visit this directly.
        // It's handled by the transformer.
    }

    TemplateType getTemplateType() const { return templateType; }
    std::string getName() const { return name; }

private:
    TemplateType templateType;
    std::string name;
};
