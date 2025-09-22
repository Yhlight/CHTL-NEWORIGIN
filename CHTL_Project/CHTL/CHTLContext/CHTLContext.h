#pragma once

#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include <string>
#include <map>
#include <memory>
#include <iostream>

namespace CHTL {

class CHTLContext {
public:
    void addTemplate(const std::string& name, std::shared_ptr<TemplateDefinitionNode> node) {
        templates[name] = node;
    }

    std::shared_ptr<TemplateDefinitionNode> getTemplate(const std::string& name) {
        if (templates.count(name)) {
            return templates[name];
        }
        return nullptr;
    }

    void addCustom(const std::string& name, std::shared_ptr<CustomDefinitionNode> node) {
        customs[name] = node;
    }

    std::shared_ptr<CustomDefinitionNode> getCustom(const std::string& name) {
        if (customs.count(name)) {
            return customs[name];
        }
        return nullptr;
    }

private:
    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>> templates;
    std::map<std::string, std::shared_ptr<CustomDefinitionNode>> customs;

public:
    void dump() {
        std::cout << "--- Context Dump ---" << std::endl;
        std::cout << "Templates:" << std::endl;
        for (const auto& t : templates) {
            std::cout << "  - " << t.first << std::endl;
        }
        std::cout << "Customs:" << std::endl;
        for (const auto& c : customs) {
            std::cout << "  - " << c.first << std::endl;
        }
        std::cout << "--------------------" << std::endl;
    }
};

} // namespace CHTL
