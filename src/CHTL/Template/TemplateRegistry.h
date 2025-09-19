#ifndef TEMPLATE_REGISTRY_H
#define TEMPLATE_REGISTRY_H

#include "CHTL/CHTLNode/TemplateNode.h"
#include <string>
#include <map>
#include <vector>

namespace CHTL {

// For now, we only handle style templates.
using StyleTemplateContent = std::vector<Attribute>;

class TemplateRegistry {
public:
    void registerStyleTemplate(const std::string& name, const StyleTemplateContent& content);
    const StyleTemplateContent* getStyleTemplate(const std::string& name) const;

private:
    std::map<std::string, StyleTemplateContent> style_templates;
    // Later, add maps for element_templates and var_templates.
};

} // namespace CHTL

#endif // TEMPLATE_REGISTRY_H
