#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class TemplateType {
    Style,
    Element,
    Var
};

// Represents a [Template] @Type Name { ... } definition
class TemplateDefinitionNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> body;

    TemplateDefinitionNode(TemplateType type, const std::string& n) : templateType(type), name(n) {}

    NodeType getType() const override { return NodeType::TemplateDefinition; }

    std::string toString(int indentation = 0) const override {
        return std::string(indentation, ' ') + "[Template] " + name;
    }
};

// Represents a @Type Name; usage
class TemplateUsageNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;

    TemplateUsageNode(TemplateType type, const std::string& n) : templateType(type), name(n) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }

    std::string toString(int indentation = 0) const override {
        return std::string(indentation, ' ') + "@TemplateUsage " + name;
    }
};

} // namespace CHTL
