#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declare to avoid circular dependencies if context needs to hold nodes
struct TemplateDefinitionNode;
struct CustomDefinitionNode;
struct CssRuleNode;

class CHTLContext {
public:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> styleTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> elementTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> varTemplates;

    std::map<std::string, std::shared_ptr<CustomDefinitionNode>> styleCustoms;
    std::map<std::string, std::shared_ptr<CustomDefinitionNode>> elementCustoms;
    std::map<std::string, std::shared_ptr<CustomDefinitionNode>> varCustoms;

    // To store parsed CSS rules that need evaluation
    std::vector<std::shared_ptr<CssRuleNode>> globalCssRuleNodes;
    // To store the final, evaluated CSS rule strings for the generator
    std::vector<std::string> finalGlobalCssRules;

    // Flag for HTML5 doctype generation
    bool useHtml5Doctype = false;
};
