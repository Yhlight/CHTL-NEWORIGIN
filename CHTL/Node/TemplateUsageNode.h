#pragma once

#include "BaseNode.h"
#include <string>
#include <sstream>
#include <vector>

namespace CHTL {

enum class TemplateType {
    Style,
    Element,
    Var
};

class TemplateUsageNode : public BaseNode {
public:
    TemplateType template_type;
    std::string name;
    std::vector<std::string> deleted_properties; // This was for Phase 6, but I'll add it back as it's simple

    TemplateUsageNode(TemplateType type, const std::string& n) : template_type(type), name(n) {}

    NodeType getType() const override {
        return NodeType::TemplateUsage;
    }

    std::string toString(int indent = 0) const override {
        std::string ind(indent * 2, ' ');
        std::stringstream ss;
        std::string type_str;
        switch(template_type) {
            case TemplateType::Style: type_str = "Style"; break;
            case TemplateType::Element: type_str = "Element"; break;
            case TemplateType::Var: type_str = "Var"; break;
        }
        ss << ind << "TemplateUsageNode(@" << type_str << " " << name << ")\n";
        return ss.str();
    }
};

} // namespace CHTL
