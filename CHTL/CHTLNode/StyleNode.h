#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include "BaseNode.h"
#include "CssRuleNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <memory>
#include <variant>
#include <algorithm>
#include "../CHTLLexer/Token.h"

// Represents a single branch of a conditional property: `condition ? "true_value"`
struct ConditionalBranch {
    std::vector<Token> condition;
    std::string trueValue;
};

// Represents a full conditional property value, which can have multiple branches and an optional else value.
struct ConditionalPropertyValue {
    std::vector<ConditionalBranch> branches;
    std::string elseValue;
};

using PropertyValue = std::variant<std::string, ConditionalPropertyValue>;

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    void addProperty(const std::string& key, PropertyValue value) {
        properties.push_back({key, std::move(value)});
    }

    void addProperty(const std::string& key, const std::string& value) {
        properties.push_back({key, value});
    }

    void deleteProperty(const std::string& key) {
        properties.erase(
            std::remove_if(properties.begin(), properties.end(),
                           [&](const auto& prop) { return prop.first == key; }),
            properties.end()
        );
    }

    void addRule(std::unique_ptr<CssRuleNode> rule) {
        rules.push_back(std::move(rule));
    }

    std::string toString(int depth = 0) const override {
        // This will be updated later to handle hoisting.
        // For now, it only represents inline styles.
        return "";
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    const std::vector<std::pair<std::string, PropertyValue>>& getProperties() const {
        return properties;
    }

    const std::vector<std::unique_ptr<CssRuleNode>>& getRules() const {
        return rules;
    }

    std::string getNodeType() const override {
        return "[Style]";
    }

private:
    std::vector<std::pair<std::string, PropertyValue>> properties;
    std::vector<std::unique_ptr<CssRuleNode>> rules;
};

#endif //CHTL_STYLE_NODE_H