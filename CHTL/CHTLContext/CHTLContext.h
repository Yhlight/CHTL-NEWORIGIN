#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "../CHTLNode/TemplateStyleNode.h"
#include "../CHTLNode/TemplateElementNode.h"
#include "../CHTLNode/TemplateVarNode.h"
#include "../CHTLNode/CustomStyleNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomVarNode.h"
#include <string>
#include <map>
#include <memory>

class CHTLContext {
public:
    CHTLContext() = default;

    void registerStyleTemplate(std::unique_ptr<TemplateStyleNode> node) {
        styleTemplates[node->getName()] = std::move(node);
    }

    void registerElementTemplate(std::unique_ptr<TemplateElementNode> node) {
        elementTemplates[node->getName()] = std::move(node);
    }

    void registerVarTemplate(std::unique_ptr<TemplateVarNode> node) {
        varTemplates[node->getName()] = std::move(node);
    }

    const TemplateStyleNode* getStyleTemplate(const std::string& name) const {
        auto it = styleTemplates.find(name);
        return (it != styleTemplates.end()) ? it->second.get() : nullptr;
    }

    const TemplateElementNode* getElementTemplate(const std::string& name) const {
        auto it = elementTemplates.find(name);
        return (it != elementTemplates.end()) ? it->second.get() : nullptr;
    }

    const TemplateVarNode* getVarTemplate(const std::string& name) const {
        auto it = varTemplates.find(name);
        return (it != varTemplates.end()) ? it->second.get() : nullptr;
    }

    void registerCustomStyle(std::unique_ptr<CustomStyleNode> node) {
        customStyleTemplates[node->getName()] = std::move(node);
    }

    void registerCustomElement(std::unique_ptr<CustomElementNode> node) {
        customElementTemplates[node->getName()] = std::move(node);
    }

    void registerCustomVar(std::unique_ptr<CustomVarNode> node) {
        customVarTemplates[node->getName()] = std::move(node);
    }

    const CustomStyleNode* getCustomStyle(const std::string& name) const {
        auto it = customStyleTemplates.find(name);
        return (it != customStyleTemplates.end()) ? it->second.get() : nullptr;
    }

    const CustomElementNode* getCustomElement(const std::string& name) const {
        auto it = customElementTemplates.find(name);
        return (it != customElementTemplates.end()) ? it->second.get() : nullptr;
    }

    const CustomVarNode* getCustomVar(const std::string& name) const {
        auto it = customVarTemplates.find(name);
        return (it != customVarTemplates.end()) ? it->second.get() : nullptr;
    }

private:
    std::map<std::string, std::unique_ptr<TemplateStyleNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<TemplateElementNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<TemplateVarNode>> varTemplates;

    std::map<std::string, std::unique_ptr<CustomStyleNode>> customStyleTemplates;
    std::map<std::string, std::unique_ptr<CustomElementNode>> customElementTemplates;
    std::map<std::string, std::unique_ptr<CustomVarNode>> customVarTemplates;
};

#endif //CHTL_CONTEXT_H