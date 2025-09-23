#include "TemplateManager.h"
#include "../Util/NodeCloner.h" // For cloning nodes during merge
#include <stdexcept>
#include <algorithm> // For std::find
#include <vector>

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

// --- Resolution Implementation ---

void TemplateManager::resolveElementParents(const std::string& ns, const std::string& name, std::vector<std::unique_ptr<BaseNode>>& finalChildren, std::vector<std::string>& visited) {
    // Cycle detection
    if (std::find(visited.begin(), visited.end(), name) != visited.end()) {
        throw std::runtime_error("Circular dependency in @Element template inheritance: " + name);
    }
    visited.push_back(name);

    ElementTemplateNode* tmpl = getElementTemplate(ns, name);
    if (!tmpl) {
        throw std::runtime_error("Element template '" + name + "' not found in namespace '" + ns + "'.");
    }

    // Recurse on parents first
    for (const auto& parentName : tmpl->parentNames) {
        resolveElementParents(ns, parentName, finalChildren, visited);
    }

    // Then, add this template's own children
    for (const auto& child : tmpl->children) {
        finalChildren.push_back(NodeCloner::clone(child.get()));
    }

    visited.pop_back();
}

std::vector<std::unique_ptr<BaseNode>> TemplateManager::getResolvedElementChildren(const std::string& ns, const std::string& name) {
    std::vector<std::unique_ptr<BaseNode>> finalChildren;
    std::vector<std::string> visited;
    resolveElementParents(ns, name, finalChildren, visited);
    return finalChildren;
}

std::pair<bool, std::string> TemplateManager::findVariableRecursive(const std::string& ns, const std::string& templateName, const std::string& varName, std::vector<std::string>& visited) {
    // Cycle detection
    if (std::find(visited.begin(), visited.end(), templateName) != visited.end()) {
        throw std::runtime_error("Circular dependency in @Var template inheritance: " + templateName);
    }
    visited.push_back(templateName);

    VarTemplateNode* tmpl = getVarTemplate(ns, templateName);
    if (!tmpl) {
        throw std::runtime_error("Var template '" + templateName + "' not found in namespace '" + ns + "'.");
    }

    // Check current template first
    auto it = tmpl->variables.find(varName);
    if (it != tmpl->variables.end()) {
        visited.pop_back();
        return {true, it->second};
    }

    // Recurse on parents
    for (const auto& parentName : tmpl->parentNames) {
        auto result = findVariableRecursive(ns, parentName, varName, visited);
        if (result.first) {
            visited.pop_back();
            return result;
        }
    }

    visited.pop_back();
    return {false, ""};
}

std::pair<bool, std::string> TemplateManager::getVariableFromTemplate(const std::string& ns, const std::string& templateName, const std::string& varName) {
    std::vector<std::string> visited;
    return findVariableRecursive(ns, templateName, varName, visited);
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
