#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <memory>
#include <unordered_map>
#include "../CHTLNode/TemplateStyleNode.h"

namespace CHTL {

class CHTLContext {
public:
    void registerStyleTemplate(const std::string& name, std::shared_ptr<TemplateStyleNode> node);
    std::shared_ptr<TemplateStyleNode> getStyleTemplate(const std::string& name) const;

private:
    std::unordered_map<std::string, std::shared_ptr<TemplateStyleNode>> styleTemplates;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H
