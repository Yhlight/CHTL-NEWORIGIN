#include "TemplateManager.h"

namespace CHTL {

TemplateManager& TemplateManager::getInstance() {
    static TemplateManager instance;
    return instance;
}

void TemplateManager::registerTemplate(const std::string& name, std::unique_ptr<BaseNode> template_node) {
    m_templates[name] = std::move(template_node);
}

const BaseNode* TemplateManager::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    if (it != m_templates.end()) {
        return it->second.get();
    }
    return nullptr;
}

void TemplateManager::clear() {
    m_templates.clear();
}

} // namespace CHTL