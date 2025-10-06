#pragma once

#include "../CHTLNode/TemplateNode.h"
#include <string>
#include <memory>
#include <map>

namespace CHTL {

const std::string GLOBAL_NAMESPACE = "__global__";

class GenerationContext {
public:
    void addTemplate(const std::string& name, std::shared_ptr<TemplateNode> node, const std::string& ns = GLOBAL_NAMESPACE) {
        namespaced_templates[ns][name] = node;
    }

    std::shared_ptr<TemplateNode> getTemplate(const std::string& name, const std::string& ns = GLOBAL_NAMESPACE) const {
        // First, try to find in the specified namespace
        auto ns_it = namespaced_templates.find(ns);
        if (ns_it != namespaced_templates.end()) {
            auto tpl_it = ns_it->second.find(name);
            if (tpl_it != ns_it->second.end()) {
                return tpl_it->second;
            }
        }

        // If not found, and we were looking in a specific namespace, try the global namespace as a fallback
        if (ns != GLOBAL_NAMESPACE) {
            auto global_ns_it = namespaced_templates.find(GLOBAL_NAMESPACE);
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