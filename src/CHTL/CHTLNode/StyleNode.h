#pragma once

#include "BaseNode.h"
#include "ASTVisitor.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Represents a single property, e.g., "width: 100px;"
struct StyleProperty {
    std::string key;
    std::string value;
};

// Represents a nested rule, e.g., ".box { color: red; }"
struct NestedStyleRule {
    std::string selector;
    std::vector<StyleProperty> properties;
};

// A StyleNode can contain a mix of simple properties (for inline styles)
// and nested rules (for global styles).
class StyleNode : public BaseNode {
public:
    std::vector<StyleProperty> inline_properties;
    std::vector<NestedStyleRule> nested_rules;

    StyleNode() = default;

    void addInlineProperty(const StyleProperty& prop) {
        inline_properties.push_back(prop);
    }

    void addNestedRule(const NestedStyleRule& rule) {
        nested_rules.push_back(rule);
    }

    std::string accept(ASTVisitor& visitor) override {
        return visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Style; }
};

} // namespace CHTL
