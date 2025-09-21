#pragma once

#include "CHTL/CHTLNode/Node.h"
#include <map>
#include <string>
#include <memory>

namespace CHTL {

class Context {
public:
    void addTemplate(std::shared_ptr<TemplateDeclarationNode> template_node) {
        // For now, we'll use one map. We can use the TemplateType to differentiate if needed.
        m_templates[template_node->name] = template_node;
    }

    std::shared_ptr<TemplateDeclarationNode> getTemplate(const std::string& name) {
        if (m_templates.count(name)) {
            return m_templates.at(name);
        }
        return nullptr;
    }

private:
    std::map<std::string, std::shared_ptr<TemplateDeclarationNode>> m_templates;
};

} // namespace CHTL
