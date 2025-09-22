#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> inlineStyles;
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    void setAttribute(const std::string& key, const std::string& value) {
        attributes[key] = value;
    }

    void addStyle(const std::string& key, const std::string& value) {
        inlineStyles[key] = value;
    }

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }
};
