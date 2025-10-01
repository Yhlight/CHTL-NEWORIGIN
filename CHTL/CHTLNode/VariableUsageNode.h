#ifndef CHTL_VARIABLEUSAGENODE_H
#define CHTL_VARIABLEUSAGENODE_H

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class VariableUsageNode : public ExpressionNode {
public:
    VariableUsageNode(const std::string& groupName, const std::string& variableName);
    ~VariableUsageNode() override = default;
    VariableUsageNode(VariableUsageNode&&) noexcept = default;
    VariableUsageNode& operator=(VariableUsageNode&&) noexcept = default;

    ExpressionType getType() const override;
    void print(int indent = 0) const override;

    std::string getGroupName() const;
    std::string getVariableName() const;

private:
    std::string groupName;
    std::string variableName;
};

} // namespace CHTL

#endif // CHTL_VARIABLEUSAGENODE_H