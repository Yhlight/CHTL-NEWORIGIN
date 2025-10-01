#include "TemplateUsageNode.h"
#include <iostream>

namespace CHTL {

TemplateUsageNode::TemplateUsageNode(const std::string& type, const std::string& name)
    : templateType(type), templateName(name) {}

void TemplateUsageNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "TemplateUsage: @" << templateType << " " << templateName << ";\n";
}

std::string TemplateUsageNode::getTemplateType() const {
    return templateType;
}

std::string TemplateUsageNode::getTemplateName() const {
    return templateName;
}

} // namespace CHTL