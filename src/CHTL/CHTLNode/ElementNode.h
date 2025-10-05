#ifndef CHTL_ELEMENT_NODE_H
#define CHTL_ELEMENT_NODE_H

#include "Node.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

class ElementNode : public Node {
public:
    explicit ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

    void addChild(std::unique_ptr<Node> child) {
        children.push_back(std::move(child));
    }

    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    std::string toString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "ElementNode(" << tagName << "):\n";
        for (const auto& attr : attributes) {
            ss << std::string(indent + 2, ' ') << "Attribute: " << attr.first << " = " << attr.second << "\n";
        }
        for (const auto& child : children) {
            ss << child->toString(indent + 2) << "\n";
        }
        return ss.str();
    }

    const std::string& getTagName() const { return tagName; }
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    const std::vector<std::unique_ptr<Node>>& getChildren() const { return children; }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<Node>> children;
};

#endif // CHTL_ELEMENT_NODE_H