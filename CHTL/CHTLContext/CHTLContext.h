#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <map>
#include <memory>
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"

namespace CHTL {

class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();

    void addTemplate(const std::string& name, std::unique_ptr<TemplateNode> node);
    const TemplateNode* getTemplate(const std::string& name) const;

    void addCustom(const std::string& name, std::unique_ptr<CustomNode> node);
    const CustomNode* getCustom(const std::string& name) const;

private:
    std::map<std::string, std::unique_ptr<TemplateNode>> templates;
    std::map<std::string, std::unique_ptr<CustomNode>> customs;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H