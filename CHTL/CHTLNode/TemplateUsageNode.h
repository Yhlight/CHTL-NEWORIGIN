#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class TemplateUsageType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(const std::string& name, TemplateUsageType usageType, const std::string& varName = "")
        : name(name), usageType(usageType), variableName(varName) {
        type = NodeType::NODE_TEMPLATE_USAGE;
    }

    const std::string& getName() const { return name; }
    TemplateUsageType getUsageType() const { return usageType; }
    const std::string& getVariableName() const { return variableName; }

private:
    std::string name;
    TemplateUsageType usageType;
    std::string variableName;
};

}