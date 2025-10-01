#ifndef CHTL_TEMPLATE_MANAGER_H
#define CHTL_TEMPLATE_MANAGER_H

#include <string>
#include <memory>
#include <map>
#include "CHTLNode/TemplateDefinitionNode.h"

namespace CHTL {

class TemplateManager {
public:
    static TemplateManager& getInstance();

    void registerTemplate(std::unique_ptr<TemplateDefinitionNode> template_node);
    const TemplateDefinitionNode* getTemplate(const std::string& name) const;

private:
    TemplateManager() = default;
    std::map<std::string, std::unique_ptr<TemplateDefinitionNode>> m_templates;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_MANAGER_H