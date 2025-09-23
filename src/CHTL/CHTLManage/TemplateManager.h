#pragma once

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
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

private:
    // Use type aliases for cleaner nested map declarations
    using StyleTemplateMap = std::map<std::string, std::unique_ptr<StyleTemplateNode>>;
    std::map<std::string, StyleTemplateMap> styleTemplates;

    using ElementTemplateMap = std::map<std::string, std::unique_ptr<ElementTemplateNode>>;
    std::map<std::string, ElementTemplateMap> elementTemplates;

    using VarTemplateMap = std::map<std::string, std::unique_ptr<VarTemplateNode>>;
    std::map<std::string, VarTemplateMap> varTemplates;
};
