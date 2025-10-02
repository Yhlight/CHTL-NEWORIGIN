#ifndef CHTL_CUSTOM_STYLE_NODE_H
#define CHTL_CUSTOM_STYLE_NODE_H

#include "TemplateStyleNode.h"
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

private:
    std::unordered_set<std::string> deletedProperties;
};

#endif // CHTL_CUSTOM_STYLE_NODE_H