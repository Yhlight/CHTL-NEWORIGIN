#include "TemplateNode.h"
#include <iostream>

namespace CHTL {

TemplateNode::TemplateNode(const std::string& type, const std::string& name)
    : templateType(type), templateName(name) {}

void TemplateNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "TemplateDefinition: @" << templateType << " " << templateName << " {\n";
    for (const auto& prop : properties) {
        prop->print(indent + 1);
    }
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "}\n";
}

std::string TemplateNode::getTemplateType() const {
    return templateType;
}

std::string TemplateNode::getTemplateName() const {
    return templateName;
}

const std::vector<std::unique_ptr<StylePropertyNode>>& TemplateNode::getProperties() const {
    return properties;
}

void TemplateNode::addProperty(std::unique_ptr<StylePropertyNode> property) {
    properties.push_back(std::move(property));
}

} // namespace CHTL