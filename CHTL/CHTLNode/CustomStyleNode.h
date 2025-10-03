#ifndef CHTL_CUSTOM_STYLE_NODE_H
#define CHTL_CUSTOM_STYLE_NODE_H

#include "TemplateStyleNode.h"
#include "NodeVisitor.h"
#include <string>
#include <vector>
#include <unordered_set>

class CustomStyleNode : public TemplateStyleNode {
public:
    explicit CustomStyleNode(const std::string& name)
        : TemplateStyleNode(name) {}

    void addDeletedProperty(const std::string& propName) {
        deletedProperties.insert(propName);
    }

    const std::unordered_set<std::string>& getDeletedProperties() const {
        return deletedProperties;
    }

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto clonedNode = std::make_unique<CustomStyleNode>(getName());
        for (const auto& prop : getProperties()) {
            clonedNode->addProperty(prop.first, prop.second);
        }
        for (const auto& deletedProp : deletedProperties) {
            clonedNode->addDeletedProperty(deletedProp);
        }
        return clonedNode;
    }

private:
    std::unordered_set<std::string> deletedProperties;
};

#endif // CHTL_CUSTOM_STYLE_NODE_H