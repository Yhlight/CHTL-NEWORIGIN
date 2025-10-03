#ifndef CHTL_CSS_RULE_NODE_H
#define CHTL_CSS_RULE_NODE_H

#include "BaseNode.h"
#include "NodeVisitor.h"
#include <string>
#include <map>
#include <sstream>

class CssRuleNode : public BaseNode {
public:
    explicit CssRuleNode(std::string selector) : selector(std::move(selector)) {}

    void addProperty(const std::string& key, const std::string& value) {
        properties[key] = value;
    }

    const std::string& getSelector() const {
        return selector;
    }

    const std::map<std::string, std::string>& getProperties() const {
        return properties;
    }

    std::string toString(int depth = 0) const override {
        // This node itself doesn't render directly, its contents are hoisted.
        return "";
    }

    std::string getNodeType() const override {
        return "[CssRule]";
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

private:
    std::string selector;
    std::map<std::string, std::string> properties;
};

#endif // CHTL_CSS_RULE_NODE_H