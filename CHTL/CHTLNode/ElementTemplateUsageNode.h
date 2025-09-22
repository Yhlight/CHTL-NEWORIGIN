#pragma once

#include "Node.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum

class ElementTemplateUsageNode : public Node {
public:
    ElementTemplateUsageNode(const std::string& name)
        : name(name) {}

    void accept(Visitor& visitor) override {
        // The generator will not visit this directly.
        // It's handled by the transformer.
    }

    TemplateType getTemplateType() const { return TemplateType::Element; }
    std::string getName() const { return name; }

private:
    std::string name;
};
