#pragma once

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/OriginNode.h"
#include <string>
#include <map>
#include <memory>

// Manages the storage and retrieval of all defined templates,
// organized by namespace.
class TemplateManager {
public:
    void addStyleTemplate(const std::string& ns, const std::string& name, std::unique_ptr<StyleTemplateNode> node);
    StyleTemplateNode* getStyleTemplate(const std::string& ns, const std::string& name);

    void addElementTemplate(const std::string& ns, const std::string& name, std::unique_ptr<ElementTemplateNode> node);
    ElementTemplateNode* getElementTemplate(const std::string& ns, const std::string& name);

    void addVarTemplate(const std::string& ns, const std::string& name, std::unique_ptr<VarTemplateNode> node);
    VarTemplateNode* getVarTemplate(const std::string& ns, const std::string& name);

    void addNamedOrigin(const std::string& ns, const std::string& name, std::unique_ptr<OriginNode> node);
    OriginNode* getNamedOrigin(const std::string& ns, const std::string& name);

    // Merges templates from another manager into this one.
    void merge(const TemplateManager& other);

    // --- Resolution Methods ---
    // These methods resolve the inheritance chain for a given template.

    // Recursively resolves @Element inheritance and returns a vector of the final child nodes.
    std::vector<std::unique_ptr<BaseNode>> getResolvedElementChildren(const std::string& ns, const std::string& name);

    // Recursively finds a variable in a @Var template's inheritance chain.
    // Returns a pair of (found, value).
    std::pair<bool, std::string> getVariableFromTemplate(const std::string& ns, const std::string& templateName, const std::string& varName);

private:
    // --- Private Recursive Helpers for Resolution ---
    void resolveElementParents(const std::string& ns, const std::string& name, std::vector<std::unique_ptr<BaseNode>>& finalChildren, std::vector<std::string>& visited);
    std::pair<bool, std::string> findVariableRecursive(const std::string& ns, const std::string& templateName, const std::string& varName, std::vector<std::string>& visited);

    // Use type aliases for cleaner nested map declarations
    using StyleTemplateMap = std::map<std::string, std::unique_ptr<StyleTemplateNode>>;
    std::map<std::string, StyleTemplateMap> styleTemplates;

    using ElementTemplateMap = std::map<std::string, std::unique_ptr<ElementTemplateNode>>;
    std::map<std::string, ElementTemplateMap> elementTemplates;

    using VarTemplateMap = std::map<std::string, std::unique_ptr<VarTemplateNode>>;
    std::map<std::string, VarTemplateMap> varTemplates;

    using NamedOriginMap = std::map<std::string, std::unique_ptr<OriginNode>>;
    std::map<std::string, NamedOriginMap> namedOrigins;
};
