#pragma once

#include "StyleStatementNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum

class StyleTemplateUsageNode : public StyleStatementNode {
public:
    StyleTemplateUsageNode(TemplateType type, const std::string& name, StyleStatementList body = {})
        : templateType(type), name(name), body(std::move(body)) {}

    void accept(Visitor& visitor) override {
        // The generator will not visit this directly.
        // It's handled by the transformer.
    }

    TemplateType getTemplateType() const { return templateType; }
    std::string getName() const { return name; }
    const StyleStatementList& getBody() const { return body; }

private:
    TemplateType templateType;
    std::string name;
    StyleStatementList body;
};
