#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "BaseNode.h"
#include "Visitor.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

// Represents an element in CHTL, like 'div { ... }'.
// It contains a tag name (e.g., "div") and a list of child nodes.
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    const std::string& getTagName() const { return tagName; }
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const { return children; }
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void addAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};

#endif // CHTL_ELEMENT_NODE_H
