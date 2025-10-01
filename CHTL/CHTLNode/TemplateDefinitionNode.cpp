#include "TemplateDefinitionNode.h"
#include <iostream>

namespace CHTL {

TemplateDefinitionNode::TemplateDefinitionNode(TemplateType type, const std::string& name)
    : m_type(type), m_name(name) {}

void TemplateDefinitionNode::addChild(std::unique_ptr<BaseNode> child) {
    m_children.push_back(std::move(child));
}

void TemplateDefinitionNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::string type_str;
    switch (m_type) {
        case TemplateType::STYLE: type_str = "Style"; break;
        case TemplateType::ELEMENT: type_str = "Element"; break;
        case TemplateType::VAR: type_str = "Var"; break;
    }
    std::cout << "Template Definition: " << type_str << " " << m_name << " {" << std::endl;
    for (const auto& child : m_children) {
        child->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
    std::cout << "}" << std::endl;
}

TemplateType TemplateDefinitionNode::getTemplateType() const {
    return m_type;
}

const std::string& TemplateDefinitionNode::getName() const {
    return m_name;
}

const std::vector<std::unique_ptr<BaseNode>>& TemplateDefinitionNode::getChildren() const {
    return m_children;
}

} // namespace CHTL