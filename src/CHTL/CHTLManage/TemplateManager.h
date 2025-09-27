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

    enum class ImportType { ALL, TEMPLATE, CUSTOM, ORIGIN, CONFIG };
    struct MergeOptions {
        ImportType type = ImportType::ALL;
        std::string subType; // e.g., "Element", "Style", "Var"
    };
    void merge(const TemplateManager& other, const MergeOptions& options);

    // Merges templates, applying a default namespace if the source has no explicit namespaces.
    void merge_with_default_namespace(const TemplateManager& other, const std::string& default_ns);


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
