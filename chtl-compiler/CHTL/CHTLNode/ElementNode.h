#pragma once

#include "Node.h"
#include <string>
#include <map>

class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    std::string tagName;
    std::map<std::string, std::string> attributes;
    NodeList children;

    NodePtr clone() const override {
        auto new_node = std::make_shared<ElementNode>(tagName);
        new_node->attributes = this->attributes;
        for (const auto& child : children) {
            new_node->children.push_back(child->clone());
        }
        return new_node;
    }
};