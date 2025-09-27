#include "TemplateManager.h"
#include "../Util/NodeCloner.h" // For cloning nodes during merge
#include <stdexcept>

// --- Style Templates ---

void TemplateManager::addStyleTemplate(const std::string& ns, const std::string& name, std::unique_ptr<StyleTemplateNode> node) {
    styleTemplates[ns][name] = std::move(node);
}

StyleTemplateNode* TemplateManager::getStyleTemplate(const std::string& ns, const std::string& name) {
    // Phase 1: Search from the current namespace up to global.
    std::string current_ns = ns;
    do {
        auto ns_it = styleTemplates.find(current_ns);
        if (ns_it != styleTemplates.end()) {
            auto name_it = ns_it->second.find(name);
            if (name_it != ns_it->second.end()) {
                return name_it->second.get();
            }
        }

        if (current_ns == "_global") break;

        size_t last_dot = current_ns.find_last_of('.');
        if (last_dot != std::string::npos) {
            current_ns = current_ns.substr(0, last_dot);
        } else {
            current_ns = "_global";
        }
    } while (true);

    // Phase 2: If not found, search all other namespaces.
    StyleTemplateNode* found_node = nullptr;
    for (const auto& ns_pair : styleTemplates) {
        if (ns.rfind(ns_pair.first, 0) == 0 || ns_pair.first == "_global") {
            continue;
        }
        auto name_it = ns_pair.second.find(name);
        if (name_it != ns_pair.second.end()) {
            if (found_node != nullptr) {
                throw std::runtime_error("Ambiguous style template reference for '" + name + "'.");
            }
            found_node = name_it->second.get();
        }
    }

    return found_node;
}

// --- Named Origins ---

void TemplateManager::addNamedOrigin(const std::string& ns, const std::string& name, std::unique_ptr<OriginNode> node) {
    namedOrigins[ns][name] = std::move(node);
}

OriginNode* TemplateManager::getNamedOrigin(const std::string& ns, const std::string& name) {
    // Phase 1: Search from the current namespace up to global.
    std::string current_ns = ns;
    do {
        auto ns_it = namedOrigins.find(current_ns);
        if (ns_it != namedOrigins.end()) {
            auto name_it = ns_it->second.find(name);
            if (name_it != ns_it->second.end()) {
                return name_it->second.get();
            }
        }

        if (current_ns == "_global") break;

        size_t last_dot = current_ns.find_last_of('.');
        if (last_dot != std::string::npos) {
            current_ns = current_ns.substr(0, last_dot);
        } else {
            current_ns = "_global";
        }
    } while (true);

    // Phase 2: If not found, search all other namespaces.
    OriginNode* found_node = nullptr;
    for (const auto& ns_pair : namedOrigins) {
        if (ns.rfind(ns_pair.first, 0) == 0 || ns_pair.first == "_global") {
            continue;
        }
        auto name_it = ns_pair.second.find(name);
        if (name_it != ns_pair.second.end()) {
            if (found_node != nullptr) {
                throw std::runtime_error("Ambiguous named origin reference for '" + name + "'.");
            }
            found_node = name_it->second.get();
        }
    }

    return found_node;
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

void TemplateManager::merge_with_default_namespace(const TemplateManager& other, const std::string& default_ns) {
    // Check if the 'other' manager has any non-global namespaces.
    bool has_explicit_namespaces = false;
    if (other.styleTemplates.size() > 1 || (other.styleTemplates.count("_global") == 0 && !other.styleTemplates.empty())) has_explicit_namespaces = true;
    if (other.elementTemplates.size() > 1 || (other.elementTemplates.count("_global") == 0 && !other.elementTemplates.empty())) has_explicit_namespaces = true;
    if (other.varTemplates.size() > 1 || (other.varTemplates.count("_global") == 0 && !other.varTemplates.empty())) has_explicit_namespaces = true;
    if (other.namedOrigins.size() > 1 || (other.namedOrigins.count("_global") == 0 && !other.namedOrigins.empty())) has_explicit_namespaces = true;

    // If it has its own namespaces, merge them directly.
    if (has_explicit_namespaces) {
        merge(other);
        return;
    }

    // Otherwise, merge all of its global content into the default_ns.
    const auto& global_styles = other.styleTemplates.find("_global");
    if (global_styles != other.styleTemplates.end()) {
        for (const auto& pair : global_styles->second) {
            addStyleTemplate(default_ns, pair.first, NodeCloner::clone_unique(pair.second.get()));
        }
    }

    const auto& global_elements = other.elementTemplates.find("_global");
    if (global_elements != other.elementTemplates.end()) {
        for (const auto& pair : global_elements->second) {
            addElementTemplate(default_ns, pair.first, NodeCloner::clone_unique(pair.second.get()));
        }
    }

    const auto& global_vars = other.varTemplates.find("_global");
    if (global_vars != other.varTemplates.end()) {
        for (const auto& pair : global_vars->second) {
            addVarTemplate(default_ns, pair.first, NodeCloner::clone_unique(pair.second.get()));
        }
    }

    const auto& global_origins = other.namedOrigins.find("_global");
    if (global_origins != other.namedOrigins.end()) {
        for (const auto& pair : global_origins->second) {
            addNamedOrigin(default_ns, pair.first, NodeCloner::clone_unique(pair.second.get()));
        }
    }
}


// --- Element Templates ---

void TemplateManager::addElementTemplate(const std::string& ns, const std::string& name, std::unique_ptr<ElementTemplateNode> node) {
    elementTemplates[ns][name] = std::move(node);
}

ElementTemplateNode* TemplateManager::getElementTemplate(const std::string& ns, const std::string& name) {
    // Phase 1: Search from the current namespace up to global.
    std::string current_ns = ns;
    do {
        auto ns_it = elementTemplates.find(current_ns);
        if (ns_it != elementTemplates.end()) {
            auto name_it = ns_it->second.find(name);
            if (name_it != ns_it->second.end()) {
                return name_it->second.get();
            }
        }

        if (current_ns == "_global") break;

        size_t last_dot = current_ns.find_last_of('.');
        if (last_dot != std::string::npos) {
            current_ns = current_ns.substr(0, last_dot);
        } else {
            current_ns = "_global";
        }
    } while (true);

    // Phase 2: If not found, search all other namespaces to simulate a shared context.
    ElementTemplateNode* found_node = nullptr;
    for (const auto& ns_pair : elementTemplates) {
        // Skip the namespaces we already checked. A simple check is if the original `ns` starts with the key.
        // Also skip _global as it was the last one checked in the loop above.
        if (ns.rfind(ns_pair.first, 0) == 0 || ns_pair.first == "_global") {
            continue;
        }

        auto name_it = ns_pair.second.find(name);
        if (name_it != ns_pair.second.end()) {
            if (found_node != nullptr) {
                // Ambiguity detected. The user must use 'from' to specify the namespace.
                throw std::runtime_error("Ambiguous template reference for '" + name + "'. It exists in multiple imported namespaces. Please use 'from' to specify.");
            }
            found_node = name_it->second.get();
        }
    }

    return found_node;
}

// --- Variable Templates ---

void TemplateManager::addVarTemplate(const std::string& ns, const std::string& name, std::unique_ptr<VarTemplateNode> node) {
    varTemplates[ns][name] = std::move(node);
}

VarTemplateNode* TemplateManager::getVarTemplate(const std::string& ns, const std::string& name) {
    // Phase 1: Search from the current namespace up to global.
    std::string current_ns = ns;
    do {
        auto ns_it = varTemplates.find(current_ns);
        if (ns_it != varTemplates.end()) {
            auto name_it = ns_it->second.find(name);
            if (name_it != ns_it->second.end()) {
                return name_it->second.get();
            }
        }

        if (current_ns == "_global") break;

        size_t last_dot = current_ns.find_last_of('.');
        if (last_dot != std::string::npos) {
            current_ns = current_ns.substr(0, last_dot);
        } else {
            current_ns = "_global";
        }
    } while (true);

    // Phase 2: If not found, search all other namespaces.
    VarTemplateNode* found_node = nullptr;
    for (const auto& ns_pair : varTemplates) {
        if (ns.rfind(ns_pair.first, 0) == 0 || ns_pair.first == "_global") {
            continue;
        }
        auto name_it = ns_pair.second.find(name);
        if (name_it != ns_pair.second.end()) {
            if (found_node != nullptr) {
                throw std::runtime_error("Ambiguous var template reference for '" + name + "'.");
            }
            found_node = name_it->second.get();
        }
    }
    return found_node;
}