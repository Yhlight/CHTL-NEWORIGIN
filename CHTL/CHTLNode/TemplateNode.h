#pragma once

#include "BaseNode.h"
#include "ElementNode.h" // For StyleProperty
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Base class for all template definitions.
class TemplateDefinitionNode : public BaseNode {
public:
    explicit TemplateDefinitionNode(std::string name) : name(std::move(name)) {}

    void accept(NodeVisitor& visitor) const override {
        // Template definitions don't get "visited" in the traditional sense
        // as they are not directly part of the output tree.
        // The generator will handle them through the context.
    }

    std::string name;
};


// Represents a [Template] @Style definition.
class StyleTemplateNode : public TemplateDefinitionNode {
public:
    using TemplateDefinitionNode::TemplateDefinitionNode; // Inherit constructor

    std::vector<StyleProperty> styles;
};


// Represents a [Template] @Element definition.
class ElementTemplateNode : public TemplateDefinitionNode {
public:
    using TemplateDefinitionNode::TemplateDefinitionNode; // Inherit constructor

    std::vector<std::unique_ptr<BaseNode>> children;
};

} // namespace CHTL
