#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateNode : public BaseNode {
public:
    TemplateNode(const std::string& name, TemplateType templateType) : BaseNode(NodeType::NODE_TEMPLATE), name(name), templateType(templateType) {}

    const std::string& getName() const { return name; }
    TemplateType getTemplateType() const { return templateType; }

private:
    std::string name;
    TemplateType templateType;
};

}