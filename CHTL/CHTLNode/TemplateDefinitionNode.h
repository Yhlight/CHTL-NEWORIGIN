#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateDefinitionNode : public BaseNode {
public:
    TemplateDefinitionNode(TemplateType type, const std::string& name)
        : type(type), name(name) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    TemplateType type;
    std::string name;
    // The `children` vector from BaseNode holds the template's body.
};
