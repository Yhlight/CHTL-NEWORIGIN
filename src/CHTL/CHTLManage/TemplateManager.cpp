#include "TemplateManager.h"
#include "../Util/NodeCloner.h" // For cloning nodes during merge
#include <stdexcept>

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

// --- Named Origins ---

void TemplateManager::addNamedOrigin(const std::string& ns, const std::string& name, std::unique_ptr<OriginNode> node) {
    namedOrigins[ns][name] = std::move(node);
}

OriginNode* TemplateManager::getNamedOrigin(const std::string& ns, const std::string& name) {
    auto ns_it = namedOrigins.find(ns);
    if (ns_it == namedOrigins.end()) {
        return nullptr;
    }
    auto name_it = ns_it->second.find(name);
    return (name_it != ns_it->second.end()) ? name_it->second.get() : nullptr;
}

void TemplateManager::merge(const TemplateManager& other) {
    // Merge Style Templates
    for (const auto& ns_pair : other.styleTemplates) {
        for (const auto& name_pair : ns_pair.second) {
            if (getStyleTemplate(ns_pair.first, name_pair.first)) {
                throw std::runtime_error("Import collision: Style template '" + name_pair.first + "' in namespace '" + ns_pair.first + "' already exists.");
            }
            auto cloned_node = NodeCloner::clone(name_pair.second.get());
            auto* raw_ptr = dynamic_cast<StyleTemplateNode*>(cloned_node.release());
            if (!raw_ptr) throw std::runtime_error("Failed to cast cloned node to StyleTemplateNode.");
            addStyleTemplate(ns_pair.first, name_pair.first, std::unique_ptr<StyleTemplateNode>(raw_ptr));
        }
    }

    // Merge Element Templates
    for (const auto& ns_pair : other.elementTemplates) {
        for (const auto& name_pair : ns_pair.second) {
            if (getElementTemplate(ns_pair.first, name_pair.first)) {
                throw std::runtime_error("Import collision: Element template '" + name_pair.first + "' in namespace '" + ns_pair.first + "' already exists.");
            }
            auto cloned_node = NodeCloner::clone(name_pair.second.get());
            auto* raw_ptr = dynamic_cast<ElementTemplateNode*>(cloned_node.release());
            if (!raw_ptr) throw std::runtime_error("Failed to cast cloned node to ElementTemplateNode.");
            addElementTemplate(ns_pair.first, name_pair.first, std::unique_ptr<ElementTemplateNode>(raw_ptr));
        }
    }

    // Merge Var Templates
    for (const auto& ns_pair : other.varTemplates) {
        for (const auto& name_pair : ns_pair.second) {
            if (getVarTemplate(ns_pair.first, name_pair.first)) {
                throw std::runtime_error("Import collision: Var template '" + name_pair.first + "' in namespace '" + ns_pair.first + "' already exists.");
            }
            auto cloned_node = NodeCloner::clone(name_pair.second.get());
            auto* raw_ptr = dynamic_cast<VarTemplateNode*>(cloned_node.release());
            if (!raw_ptr) throw std::runtime_error("Failed to cast cloned node to VarTemplateNode.");
            addVarTemplate(ns_pair.first, name_pair.first, std::unique_ptr<VarTemplateNode>(raw_ptr));
        }
    }

    // Merge Named Origins
    for (const auto& ns_pair : other.namedOrigins) {
        for (const auto& name_pair : ns_pair.second) {
            if (getNamedOrigin(ns_pair.first, name_pair.first)) {
                throw std::runtime_error("Import collision: Named origin '" + name_pair.first + "' in namespace '" + ns_pair.first + "' already exists.");
            }
            auto cloned_node = NodeCloner::clone(name_pair.second.get());
            auto* raw_ptr = dynamic_cast<OriginNode*>(cloned_node.release());
            if (!raw_ptr) throw std::runtime_error("Failed to cast cloned node to OriginNode.");
            addNamedOrigin(ns_pair.first, name_pair.first, std::unique_ptr<OriginNode>(raw_ptr));
        }
    }
}

void TemplateManager::merge(const TemplateManager& other, const MergeOptions& options) {
    if (options.type == ImportType::ALL) {
        merge(other);
        return;
    }

    bool merge_templates = (options.type == ImportType::TEMPLATE);
    bool merge_customs = (options.type == ImportType::CUSTOM);
    bool merge_origins = (options.type == ImportType::ORIGIN);

    if ((merge_templates || merge_customs) && (options.subType.empty() || options.subType == "Style")) {
        for (const auto& ns_pair : other.styleTemplates) {
            for (const auto& name_pair : ns_pair.second) {
                bool is_custom_node = name_pair.second->isCustom;
                if ((merge_templates && !is_custom_node) || (merge_customs && is_custom_node)) {
                    if (getStyleTemplate(ns_pair.first, name_pair.first)) continue;
                    auto cloned_node = NodeCloner::clone(name_pair.second.get());
                    addStyleTemplate(ns_pair.first, name_pair.first, std::unique_ptr<StyleTemplateNode>(static_cast<StyleTemplateNode*>(cloned_node.release())));
                }
            }
        }
    }

    if ((merge_templates || merge_customs) && (options.subType.empty() || options.subType == "Element")) {
        for (const auto& ns_pair : other.elementTemplates) {
            for (const auto& name_pair : ns_pair.second) {
                bool is_custom_node = name_pair.second->isCustom;
                if ((merge_templates && !is_custom_node) || (merge_customs && is_custom_node)) {
                    if (getElementTemplate(ns_pair.first, name_pair.first)) continue;
                    auto cloned_node = NodeCloner::clone(name_pair.second.get());
                    addElementTemplate(ns_pair.first, name_pair.first, std::unique_ptr<ElementTemplateNode>(static_cast<ElementTemplateNode*>(cloned_node.release())));
                }
            }
        }
    }

    if ((merge_templates || merge_customs) && (options.subType.empty() || options.subType == "Var")) {
        for (const auto& ns_pair : other.varTemplates) {
            for (const auto& name_pair : ns_pair.second) {
                bool is_custom_node = name_pair.second->isCustom;
                if ((merge_templates && !is_custom_node) || (merge_customs && is_custom_node)) {
                    if (getVarTemplate(ns_pair.first, name_pair.first)) continue;
                    auto cloned_node = NodeCloner::clone(name_pair.second.get());
                    addVarTemplate(ns_pair.first, name_pair.first, std::unique_ptr<VarTemplateNode>(static_cast<VarTemplateNode*>(cloned_node.release())));
                }
            }
        }
    }

    if (merge_origins && (options.subType.empty() || options.subType == "Origin")) {
        for (const auto& ns_pair : other.namedOrigins) {
            for (const auto& name_pair : ns_pair.second) {
                if (getNamedOrigin(ns_pair.first, name_pair.first)) continue;
                auto cloned_node = NodeCloner::clone(name_pair.second.get());
                addNamedOrigin(ns_pair.first, name_pair.first, std::unique_ptr<OriginNode>(static_cast<OriginNode*>(cloned_node.release())));
            }
        }
    }
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