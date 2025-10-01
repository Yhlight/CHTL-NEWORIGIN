#ifndef CHTL_VARIABLE_ACCESS_NODE_H
#define CHTL_VARIABLE_ACCESS_NODE_H

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class VariableAccessNode : public ExpressionNode {
public:
    VariableAccessNode(const std::string& groupName, const std::string& variableName)
        : m_groupName(groupName), m_variableName(variableName) {}

    ExpressionType getType() const override { return ExpressionType::VARIABLE_ACCESS; }
    void print(int indent = 0) const override;

    const std::string& getGroupName() const { return m_groupName; }
    const std::string& getVariableName() const { return m_variableName; }

private:
    std::string m_groupName;
    std::string m_variableName;
};

} // namespace CHTL

#endif // CHTL_VARIABLE_ACCESS_NODE_H