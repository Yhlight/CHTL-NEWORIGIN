#ifndef CHTL_VAR_TEMPLATE_USAGE_NODE_H
#define CHTL_VAR_TEMPLATE_USAGE_NODE_H

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class VarTemplateUsageNode : public ExpressionNode {
private:
    std::string templateName;
    std::string variableName;

public:
    VarTemplateUsageNode(const std::string& templateName, const std::string& variableName);
    ExpressionType getType() const override;
    void print(int indent = 0) const override;
    const std::string& getTemplateName() const;
    const std::string& getVariableName() const;
};

} // namespace CHTL

#endif // CHTL_VAR_TEMPLATE_USAGE_NODE_H