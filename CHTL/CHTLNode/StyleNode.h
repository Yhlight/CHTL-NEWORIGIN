#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include "CssRuleNode.h"
#include <vector>
#include <memory>

namespace CHTL {

class StyleNode : public BaseNode {
public:
    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const { return properties; }
    const std::vector<std::unique_ptr<CssRuleNode>>& getRules() const { return rules; }

    void addProperty(std::unique_ptr<StylePropertyNode> property) {
        properties.push_back(std::move(property));
    }
    void addRule(std::unique_ptr<CssRuleNode> rule) {
        rules.push_back(std::move(rule));
    }
    void print(int indent = 0) const override;


private:
    std::vector<std::unique_ptr<StylePropertyNode>> properties;
    std::vector<std::unique_ptr<CssRuleNode>> rules;
};

} // namespace CHTL

#endif // CHTL_STYLE_NODE_H