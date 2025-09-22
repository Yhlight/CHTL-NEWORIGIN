#pragma once

#include "BaseNode.h"
#include "CssRule.h"
#include <map>

// Represents an element like `div { ... }`
class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::shared_ptr<BaseNode>> children;
    // Attributes will be stored here, e.g., { "id" -> "main" }
    std::map<std::string, std::string> attributes;
    // Inline styles from a `style {}` block will be stored here
    std::map<std::string, std::string> inlineStyles;
    // CSS rules to be hoisted to a global style block
    std::vector<CssRule> globalCssRules;

    explicit ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::shared_ptr<BaseNode> clone() const override {
        auto new_node = std::make_shared<ElementNode>(tagName);
        new_node->attributes = this->attributes;
        new_node->inlineStyles = this->inlineStyles;
        new_node->globalCssRules = this->globalCssRules;
        for (const auto& child : this->children) {
            new_node->children.push_back(child->clone());
        }
        return new_node;
    }
};
