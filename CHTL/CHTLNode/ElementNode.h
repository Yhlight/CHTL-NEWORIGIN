#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../CHTLExpr/BaseExprNode.h"

struct TemplateUsageNode; // Forward Declaration

struct ElementNode : public BaseNode {
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::vector<std::shared_ptr<TemplateUsageNode>> styleUsages;
    std::map<std::string, std::shared_ptr<BaseExprNode>> inlineStyles;
    std::map<std::string, std::string> finalStyles;

    explicit ElementNode(const std::string& tagName) : tagName(tagName) {}

    void addAttribute(const std::shared_ptr<AttributeNode>& attribute) {
        attributes.push_back(attribute);
    }

    void addChild(const std::shared_ptr<BaseNode>& child) {
        children.push_back(child);
    }

    NodeType getType() const override { return NodeType::Element; }
};
