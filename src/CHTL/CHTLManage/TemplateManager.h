#pragma once

#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include <string>
#include <map>
#include <memory>

// Manages the storage and retrieval of all defined templates.
// An instance of this class will be owned by the Parser.
class TemplateManager {
public:
    void addStyleTemplate(const std::string& name, std::unique_ptr<StyleTemplateNode> node);
    StyleTemplateNode* getStyleTemplate(const std::string& name);

    void addElementTemplate(const std::string& name, std::unique_ptr<ElementTemplateNode> node);
    ElementTemplateNode* getElementTemplate(const std::string& name);

    void addVarTemplate(const std::string& name, std::unique_ptr<VarTemplateNode> node);
    VarTemplateNode* getVarTemplate(const std::string& name);

private:
    std::map<std::string, std::unique_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<ElementTemplateNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<VarTemplateNode>> varTemplates;
};
