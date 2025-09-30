#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "StyleNode.h"

namespace CHTL {

class ElementNode : public BaseNode {
public:
    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    const std::string& getTagName() const { return tagName; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    const StyleNode* getStyle() const { return style.get(); }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        if (it != attributes.end()) {
            return it->second;
        }
        return ""; // Return empty string if not found
    }

    void setStyle(std::unique_ptr<StyleNode> newStyle) {
        style = std::move(newStyle);
    }

private:
    std::string tagName;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::map<std::string, std::string> attributes;
    std::unique_ptr<StyleNode> style;
};

} // namespace CHTL

#endif // CHTL_ELEMENT_NODE_H