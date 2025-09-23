#include "TemplateManager.h"

// --- Style Templates ---

void TemplateManager::addStyleTemplate(const std::string& name, std::unique_ptr<StyleTemplateNode> node) {
    styleTemplates[name] = std::move(node);
}

StyleTemplateNode* TemplateManager::getStyleTemplate(const std::string& name) {
    auto it = styleTemplates.find(name);
    return (it != styleTemplates.end()) ? it->second.get() : nullptr;
}

// --- Element Templates ---

void TemplateManager::addElementTemplate(const std::string& name, std::unique_ptr<ElementTemplateNode> node) {
    elementTemplates[name] = std::move(node);
}

ElementTemplateNode* TemplateManager::getElementTemplate(const std::string& name) {
    auto it = elementTemplates.find(name);
    return (it != elementTemplates.end()) ? it->second.get() : nullptr;
}

// --- Variable Templates ---

void TemplateManager::addVarTemplate(const std::string& name, std::unique_ptr<VarTemplateNode> node) {
    varTemplates[name] = std::move(node);
}

VarTemplateNode* TemplateManager::getVarTemplate(const std::string& name) {
    auto it = varTemplates.find(name);
    return (it != varTemplates.end()) ? it->second.get() : nullptr;
}
