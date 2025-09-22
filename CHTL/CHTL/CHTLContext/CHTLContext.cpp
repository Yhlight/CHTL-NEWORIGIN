#include "CHTLContext.h"

namespace CHTL {

void CHTLContext::registerStyleTemplate(const std::string& name, std::shared_ptr<TemplateStyleNode> node) {
    styleTemplates[name] = node;
}

std::shared_ptr<TemplateStyleNode> CHTLContext::getStyleTemplate(const std::string& name) const {
    auto it = styleTemplates.find(name);
    if (it != styleTemplates.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace CHTL
