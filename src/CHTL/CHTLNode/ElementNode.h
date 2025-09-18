#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include "ASTVisitor.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<NodePtr> children;

    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void addAttribute(std::unique_ptr<AttributeNode> attr) {
        attributes.push_back(std::move(attr));
    }

    void addChild(NodePtr child) {
        children.push_back(std::move(child));
    }

    std::string accept(ASTVisitor& visitor) override {
        return visitor.visit(*this);
    }
    NodeType getType() const override { return NodeType::Element; }
};

} // namespace CHTL
