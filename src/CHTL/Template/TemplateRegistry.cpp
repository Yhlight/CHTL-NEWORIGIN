#include "TemplateRegistry.h"

namespace CHTL {

void TemplateRegistry::registerStyleTemplate(const std::string& name, const StyleTemplateContent& content) {
    style_templates[name] = content;
}

const StyleTemplateContent* TemplateRegistry::getStyleTemplate(const std::string& name) const {
    auto it = style_templates.find(name);
    if (it != style_templates.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace CHTL
