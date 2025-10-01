#ifndef CHTL_TEMPLATEUSAGENODE_H
#define CHTL_TEMPLATEUSAGENODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(const std::string& type, const std::string& name);
    ~TemplateUsageNode() override = default;
    TemplateUsageNode(TemplateUsageNode&&) noexcept = default;
    TemplateUsageNode& operator=(TemplateUsageNode&&) noexcept = default;

    void print(int indent = 0) const override;
    std::string getTemplateType() const;
    std::string getTemplateName() const;

private:
    std::string templateType;
    std::string templateName;
};

} // namespace CHTL

#endif // CHTL_TEMPLATEUSAGENODE_H