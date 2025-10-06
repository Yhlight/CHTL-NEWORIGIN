#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

enum class TemplateUsageType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(const std::string& name, TemplateUsageType usageType, const std::string& varName = "", const std::string& from_namespace = "")
        : BaseNode(NodeType::NODE_TEMPLATE_USAGE), name(name), usageType(usageType), variableName(varName), from_namespace(from_namespace) {}

    const std::string& getName() const { return name; }
    TemplateUsageType getUsageType() const { return usageType; }
    const std::string& getVariableName() const { return variableName; }
    const std::string& getFromNamespace() const { return from_namespace; }

    void setSpecialization(std::shared_ptr<BaseNode> rules) { specialization_rules = rules; }
    std::shared_ptr<BaseNode> getSpecialization() const { return specialization_rules; }

private:
    std::string name;
    TemplateUsageType usageType;
    std::string variableName;
    std::string from_namespace;
    std::shared_ptr<BaseNode> specialization_rules;
};

}