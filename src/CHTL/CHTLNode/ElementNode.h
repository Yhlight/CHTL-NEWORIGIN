#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(NodeVisitor& visitor) override {
        visitor.visit(*this);
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void addAttribute(std::unique_ptr<AttributeNode> attr) {
        attributes.push_back(std::move(attr));
    }

    std::string tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
};