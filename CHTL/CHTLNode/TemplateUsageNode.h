#ifndef CHTL_TEMPLATE_USAGE_NODE_H
#define CHTL_TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include "TemplateDefinitionNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name, std::vector<std::unique_ptr<BaseNode>> specializations = {});

    void print(int indent = 0) const override;

    TemplateType getTemplateType() const;
    const std::string& getName() const;
    const std::vector<std::unique_ptr<BaseNode>>& getSpecializations() const;

private:
    TemplateType m_type;
    std::string m_name;
    std::vector<std::unique_ptr<BaseNode>> m_specializations;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_USAGE_NODE_H