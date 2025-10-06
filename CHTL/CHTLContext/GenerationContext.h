#pragma once

#include "../CHTLNode/TemplateNode.h"
#include <string>
#include <memory>
#include <map>

namespace CHTL {

class GenerationContext {
public:
    void addTemplate(const std::string& name, std::shared_ptr<TemplateNode> node, const std::string& ns = "") {
        namespaced_templates[ns][name] = node;
    }

    std::shared_ptr<TemplateNode> getTemplate(const std::string& name, const std::string& ns = "") const {
        // Look in the specified namespace first.
        auto ns_it = namespaced_templates.find(ns);
        if (ns_it != namespaced_templates.end()) {
            auto tpl_it = ns_it->second.find(name);
            if (tpl_it != ns_it->second.end()) {
                return tpl_it->second;
            }
        }

        // If not found in the specified namespace, and the namespace was not global,
        // try falling back to the global namespace.
        if (!ns.empty()) {
            auto global_ns_it = namespaced_templates.find("");
            if (global_ns_it != namespaced_templates.end()) {
                auto tpl_it = global_ns_it->second.find(name);
                if (tpl_it != global_ns_it->second.end()) {
                    return tpl_it->second;
                }
            }
        }

        return nullptr;
    }

private:
    std::map<std::string, std::map<std::string, std::shared_ptr<TemplateNode>>> namespaced_templates;
};

}