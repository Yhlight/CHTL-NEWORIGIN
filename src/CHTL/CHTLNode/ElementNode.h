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
        auto clonedNode = std::make_unique<ElementNode>(tagName);
        clonedNode->attributes = this->attributes;
        for (const auto& child : getChildren()) {
            clonedNode->addChild(child->clone());
        }
        return clonedNode;
    }

private:
    std::string tagName;
    std::map<std::string, std::string> attributes;
};