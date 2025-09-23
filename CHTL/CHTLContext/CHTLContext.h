#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declare to avoid circular dependencies if context needs to hold nodes
struct TemplateDefinitionNode;
struct CssRuleNode;

class CHTLContext {
public:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> styleTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> elementTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> varTemplates;

    // To store parsed CSS rules that need evaluation
    std::vector<std::shared_ptr<CssRuleNode>> globalCssRuleNodes;
    // To store the final, evaluated CSS rule strings for the generator
    std::vector<std::string> finalGlobalCssRules;
};
