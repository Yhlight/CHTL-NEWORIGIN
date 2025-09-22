#pragma once

#include "../Node/BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>

namespace CHTL {

struct BaseTemplateInfo {
    std::string name;
    std::vector<std::string> deleted_properties;
};

struct StyleTemplate {
    std::string name;
    bool is_custom = false;
    std::vector<std::pair<std::string, std::string>> properties;
    std::vector<BaseTemplateInfo> base_templates;
};

struct ElementTemplate {
    std::string name;
    bool is_custom = false;
    std::vector<std::unique_ptr<BaseNode>> nodes;
    std::vector<BaseTemplateInfo> base_templates;
};

struct VarTemplate {
    std::string name;
    bool is_custom = false;
    std::map<std::string, std::string> variables;
    std::vector<BaseTemplateInfo> base_templates;
};


class TemplateStore {
public:
    void addStyleTemplate(const StyleTemplate& st) {
        style_templates[st.name] = st;
    }

    const StyleTemplate* getStyleTemplate(const std::string& name) const {
        auto it = style_templates.find(name);
        return (it != style_templates.end()) ? &it->second : nullptr;
    }

    void addElementTemplate(ElementTemplate et) {
        element_templates[et.name] = std::move(et);
    }

    const ElementTemplate* getElementTemplate(const std::string& name) const {
        auto it = element_templates.find(name);
        return (it != element_templates.end()) ? &it->second : nullptr;
    }

    void addVarTemplate(const VarTemplate& vt) {
        var_templates[vt.name] = vt;
    }

    const VarTemplate* getVarTemplate(const std::string& name) const {
        auto it = var_templates.find(name);
        return (it != var_templates.end()) ? &it->second : nullptr;
    }

private:
    std::map<std::string, StyleTemplate> style_templates;
    std::map<std::string, ElementTemplate> element_templates;
    std::map<std::string, VarTemplate> var_templates;
};

} // namespace CHTL
