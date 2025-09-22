#pragma once

#include "BaseNode.h"
#include "TemplateNode.h" // For TemplateType enum
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents a [Custom] @Type Name { ... } definition
class CustomDefinitionNode : public BaseNode {
public:
    TemplateType customType; // Re-using enum from TemplateNode.h
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> body;

    CustomDefinitionNode(TemplateType type, const std::string& n) : customType(type), name(n) {}

    NodeType getType() const override { return NodeType::CustomDefinition; }

    std::string toString(int indentation = 0) const override {
        return std::string(indentation, ' ') + "[Custom] " + name;
    }
};

// Represents a @Type Name { ... } usage for customization
class CustomUsageNode : public BaseNode {
public:
    TemplateType customType;
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> specializationBody;

    CustomUsageNode(TemplateType type, const std::string& n) : customType(type), name(n) {}

    NodeType getType() const override { return NodeType::CustomUsage; }

    std::string toString(int indentation = 0) const override {
        return std::string(indentation, ' ') + "@CustomUsage " + name;
    }
};

} // namespace CHTL
