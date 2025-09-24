#ifndef CHTL_STYLE_NODE_H
#define CHTL_STYLE_NODE_H

#include <string>
#include <vector>
#include <map>

// Represents a CSS rule block like '.selector { content }'
struct CssRule {
    std::string selector;
    std::string content; // Raw content inside the braces for now
};

// This node holds the structured content of a local style { ... } block.
class StyleNode {
public:
    const std::map<std::string, std::string>& getInlineProperties() const { return inlineProperties; }
    const std::vector<CssRule>& getCssRules() const { return cssRules; }

    void addInlineProperty(const std::string& key, const std::string& value) {
        inlineProperties[key] = value;
    }

    void addCssRule(const CssRule& rule) {
        cssRules.push_back(rule);
    }

private:
    std::map<std::string, std::string> inlineProperties;
    std::vector<CssRule> cssRules;
};

#endif // CHTL_STYLE_NODE_H
