#pragma once

#include "../CHTLNode/TemplateNode.h"
#include <string>
#include <memory>
#include <map>

namespace CHTL {

class GenerationContext {
public:
    void addTemplate(const std::string& name, std::shared_ptr<TemplateNode> node) {
        templates[name] = node;
    }

    std::shared_ptr<TemplateNode> getTemplate(const std::string& name) const {
        auto it = templates.find(name);
        if (it != templates.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    std::map<std::string, std::shared_ptr<TemplateNode>> templates;
};

}