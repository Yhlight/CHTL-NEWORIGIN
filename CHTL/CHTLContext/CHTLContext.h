#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include "../CHTLNode/TemplateStyleNode.h"
#include "../CHTLNode/TemplateElementNode.h"
#include "../CHTLNode/TemplateVarNode.h"
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

private:
    std::map<std::string, std::unique_ptr<TemplateStyleNode>> styleTemplates;
    std::map<std::string, std::unique_ptr<TemplateElementNode>> elementTemplates;
    std::map<std::string, std::unique_ptr<TemplateVarNode>> varTemplates;
};

#endif //CHTL_CONTEXT_H