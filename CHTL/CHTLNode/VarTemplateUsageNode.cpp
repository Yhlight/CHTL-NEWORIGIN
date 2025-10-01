#include "VarTemplateUsageNode.h"
#include <iostream>

namespace CHTL {

VarTemplateUsageNode::VarTemplateUsageNode(const std::string& templateName, const std::string& variableName)
    : templateName(templateName), variableName(variableName) {}

ExpressionType VarTemplateUsageNode::getType() const {
    return ExpressionType::VAR_TEMPLATE_USAGE;
}

const std::string& VarTemplateUsageNode::getTemplateName() const {
    return templateName;
}

const std::string& VarTemplateUsageNode::getVariableName() const {
    return variableName;
}

void VarTemplateUsageNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "VarTemplateUsage: " << templateName << "(" << variableName << ")" << std::endl;
}

} // namespace CHTL