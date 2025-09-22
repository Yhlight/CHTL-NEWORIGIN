#pragma once

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType enum
#include <string>

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name)
        : type(type), name(name) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    TemplateType type;
    std::string name;
};
