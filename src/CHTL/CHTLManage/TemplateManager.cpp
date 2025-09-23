#include "TemplateManager.h"

// --- Style Templates ---

void TemplateManager::addStyleTemplate(const std::string& ns, const std::string& name, std::unique_ptr<StyleTemplateNode> node) {
    styleTemplates[ns][name] = std::move(node);
}

StyleTemplateNode* TemplateManager::getStyleTemplate(const std::string& ns, const std::string& name) {
    auto ns_it = styleTemplates.find(ns);
    if (ns_it == styleTemplates.end()) {
        return nullptr;
    }
    auto name_it = ns_it->second.find(name);
    return (name_it != ns_it->second.end()) ? name_it->second.get() : nullptr;
}

// --- Element Templates ---

void TemplateManager::addElementTemplate(const std::string& ns, const std::string& name, std::unique_ptr<ElementTemplateNode> node) {
    elementTemplates[ns][name] = std::move(node);
}

ElementTemplateNode* TemplateManager::getElementTemplate(const std::string& ns, const std::string& name) {
    auto ns_it = elementTemplates.find(ns);
    if (ns_it == elementTemplates.end()) {
        return nullptr;
    }
    auto name_it = ns_it->second.find(name);
    return (name_it != ns_it->second.end()) ? name_it->second.get() : nullptr;
}

// --- Variable Templates ---

void TemplateManager::addVarTemplate(const std::string& ns, const std::string& name, std::unique_ptr<VarTemplateNode> node) {
    varTemplates[ns][name] = std::move(node);
}

VarTemplateNode* TemplateManager::getVarTemplate(const std::string& ns, const std::string& name) {
    auto ns_it = varTemplates.find(ns);
    if (ns_it == varTemplates.end()) {
        return nullptr;
    }
    auto name_it = ns_it->second.find(name);
    return (name_it != ns_it->second.end()) ? name_it->second.get() : nullptr;
}
