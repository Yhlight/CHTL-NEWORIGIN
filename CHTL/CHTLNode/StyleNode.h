#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include "StyleRuleNode.h"
#include "TemplateUsageNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const { return properties; }
    const std::vector<std::unique_ptr<StyleRuleNode>>& getRules() const { return rules; }
    const std::vector<std::unique_ptr<TemplateUsageNode>>& getTemplateUsages() const { return templateUsages; }

    void addProperty(std::unique_ptr<StylePropertyNode> property) {
        properties.push_back(std::move(property));
    }
    void addRule(std::unique_ptr<StyleRuleNode> rule) {
        rules.push_back(std::move(rule));
    }
    void addTemplateUsage(std::unique_ptr<TemplateUsageNode> usage) {
        templateUsages.push_back(std::move(usage));
    }
    void print(int indent = 0) const override;

private:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::vector<std::unique_ptr<StyleRuleNode>> rules;
    std::vector<std::unique_ptr<TemplateUsageNode>> templateUsages;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H