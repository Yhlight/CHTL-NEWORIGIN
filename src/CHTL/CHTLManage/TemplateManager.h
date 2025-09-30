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

    // Public getters to allow iteration (e.g., for cmod_packer)
    const std::map<std::string, std::map<std::string, std::unique_ptr<StyleTemplateNode>>>& getStyleTemplates() const { return styleTemplates; }
    const std::map<std::string, std::map<std::string, std::unique_ptr<ElementTemplateNode>>>& getElementTemplates() const { return elementTemplates; }
    const std::map<std::string, std::map<std::string, std::unique_ptr<VarTemplateNode>>>& getVarTemplates() const { return varTemplates; }

private:
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
