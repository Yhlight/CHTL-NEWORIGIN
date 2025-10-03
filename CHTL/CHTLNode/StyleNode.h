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

class StyleNode : public BaseNode {
public:
    StyleNode() = default;

    void addProperty(const std::string& key, const std::string& value) {
        properties.push_back({key, value});
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

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<StyleNode>();
        for (const auto& prop : properties) {
            clonedNode->addProperty(prop.first, prop.second);
        }
        for (const auto& rule : rules) {
            clonedNode->addRule(std::unique_ptr<CssRuleNode>(static_cast<CssRuleNode*>(rule->clone().release())));
        }
        return clonedNode;
    }

    const std::vector<std::pair<std::string, std::string>>& getProperties() const {
        return properties;
    }

    const std::vector<std::unique_ptr<CssRuleNode>>& getRules() const {
        return rules;
    }

    std::string getNodeType() const override {
        return "[Style]";
    }

private:
    std::vector<std::pair<std::string, std::string>> properties;
    std::vector<std::unique_ptr<CssRuleNode>> rules;
};

#endif //CHTL_STYLE_NODE_H