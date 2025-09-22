#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declare to avoid circular dependencies if context needs to hold nodes
struct TemplateDefinitionNode;

class CHTLContext {
public:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> styleTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> elementTemplates;
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> varTemplates;
};
