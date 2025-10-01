#include "TemplateUsageNode.h"
#include <iostream>

namespace CHTL {

TemplateUsageNode::TemplateUsageNode(TemplateType type, const std::string& name)
    : m_type(type), m_name(name) {}

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
    std::cout << "Template Usage: @" << type_str << " " << m_name << ";" << std::endl;
}

TemplateType TemplateUsageNode::getTemplateType() const {
    return m_type;
}

const std::string& TemplateUsageNode::getName() const {
    return m_name;
}

} // namespace CHTL