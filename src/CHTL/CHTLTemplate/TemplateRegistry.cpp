#include "TemplateRegistry.h"

namespace CHTL {

void TemplateRegistry::registerTemplate(const String& name, TemplateNode::TemplateType type,
                                       const Vector<SharedPtr<BaseNode>>& children) {
    String key = makeKey(name, type);
    TemplateDefinition def(name, type);
    def.children = children;
    templates_[key] = def;
}

void TemplateRegistry::registerVarTemplate(const String& name, const HashMap<String, String>& variables) {
    varTemplates_[name] = variables;
}

Optional<TemplateDefinition> TemplateRegistry::findTemplate(const String& name, TemplateNode::TemplateType type) {
    String key = makeKey(name, type);
    auto it = templates_.find(key);
    if (it != templates_.end()) {
        return it->second;
    }
    return std::nullopt;
}

Optional<HashMap<String, String>> TemplateRegistry::findVarTemplate(const String& name) {
    auto it = varTemplates_.find(name);
    if (it != varTemplates_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool TemplateRegistry::hasTemplate(const String& name, TemplateNode::TemplateType type) {
    String key = makeKey(name, type);
    return templates_.find(key) != templates_.end();
}

void TemplateRegistry::clear() {
    templates_.clear();
    varTemplates_.clear();
}

String TemplateRegistry::makeKey(const String& name, TemplateNode::TemplateType type) const {
    String prefix;
    switch (type) {
        case TemplateNode::TemplateType::Style:
            prefix = "style:";
            break;
        case TemplateNode::TemplateType::Element:
            prefix = "element:";
            break;
        case TemplateNode::TemplateType::Var:
            prefix = "var:";
            break;
    }
    return prefix + name;
}

} // namespace CHTL
