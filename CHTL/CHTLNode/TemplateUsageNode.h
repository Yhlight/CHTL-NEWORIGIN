#ifndef CHTL_TEMPLATE_USAGE_NODE_H
#define CHTL_TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include "TemplateDefinitionNode.h"
#include <string>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name);

    void print(int indent = 0) const override;

    TemplateType getTemplateType() const;
    const std::string& getName() const;

private:
    TemplateType m_type;
    std::string m_name;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_USAGE_NODE_H