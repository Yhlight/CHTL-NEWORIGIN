#include "VariableUsageNode.h"
#include <iostream>

namespace CHTL {

VariableUsageNode::VariableUsageNode(const std::string& groupName, const std::string& variableName)
    : groupName(groupName), variableName(variableName) {}

ExpressionType VariableUsageNode::getType() const {
    return ExpressionType::VARIABLE_USAGE;
}

void VariableUsageNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "VariableUsage: " << groupName << "(" << variableName << ")\n";
}

std::string VariableUsageNode::getGroupName() const {
    return groupName;
}

std::string VariableUsageNode::getVariableName() const {
    return variableName;
}

} // namespace CHTL