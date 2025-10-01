#include "TemplateUsageNode.h"
#include <iostream>

namespace CHTL {

TemplateUsageNode::TemplateUsageNode(TemplateType type, const std::string& name, std::vector<std::unique_ptr<BaseNode>> specializations)
    : m_type(type), m_name(name), m_specializations(std::move(specializations)) {}

void TemplateUsageNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::string type_str;
    switch (m_type) {
        case TemplateType::STYLE: type_str = "Style"; break;
        case TemplateType::ELEMENT: type_str = "Element"; break;
        case TemplateType::VAR: type_str = "Var"; break;
    }
    std::cout << "Template Usage: @" << type_str << " " << m_name;

    if (m_specializations.empty()) {
        std::cout << ";" << std::endl;
    } else {
        std::cout << " {" << std::endl;
        for (const auto& child : m_specializations) {
            child->print(indent + 1);
        }
        for (int i = 0; i < indent; ++i) {
            std::cout << "  ";
        }
        std::cout << "}" << std::endl;
    }
}

TemplateType TemplateUsageNode::getTemplateType() const {
    return m_type;
}

const std::string& TemplateUsageNode::getName() const {
    return m_name;
}

const std::vector<std::unique_ptr<BaseNode>>& TemplateUsageNode::getSpecializations() const {
    return m_specializations;
}

} // namespace CHTL