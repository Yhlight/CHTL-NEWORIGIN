#ifndef CHTL_TEMPLATE_MANAGER_H
#define CHTL_TEMPLATE_MANAGER_H

#include <string>
#include <memory>
#include <map>
#include "CHTLNode/BaseNode.h"

namespace CHTL {

class TemplateManager {
public:
    static TemplateManager& getInstance();

    void registerTemplate(const std::string& name, std::unique_ptr<BaseNode> template_node);
    const BaseNode* getTemplate(const std::string& name) const;
    void clear();

private:
    TemplateManager() = default;
    std::map<std::string, std::unique_ptr<BaseNode>> m_templates;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_MANAGER_H