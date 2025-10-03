#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    NodeType getType() const override { return NodeType::Element; }

    const std::string& getTagName() const {
        return tagName;
    }

    void setAttribute(const std::string& name, const std::string& value) {
        attributes[name] = value;
    }

    const std::map<std::string, std::string>& getAttributes() const {
        return attributes;
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ElementNode>(tagName);
        for (const auto& attr : attributes) {
            node->setAttribute(attr.first, attr.second);
        }
        for (const auto& child : getChildren()) {
            node->addChild(child->clone());
        }
        return node;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
};