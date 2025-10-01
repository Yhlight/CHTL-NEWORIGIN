#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext() {}

CHTLContext::~CHTLContext() {}

void CHTLContext::addTemplate(const std::string& name, std::unique_ptr<TemplateNode> node) {
    templates[name] = std::move(node);
}

const TemplateNode* CHTLContext::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return it->second.get();
    }
    return nullptr;
}

} // namespace CHTL