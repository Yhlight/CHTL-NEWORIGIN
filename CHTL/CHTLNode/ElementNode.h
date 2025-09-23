#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "../CHTLExpr/BaseExprNode.h"

struct TemplateUsageNode; // Forward Declaration

enum class ConstraintType {
    ExactTag,
    AnyHtml,
    AnyTemplate,
    AnyCustom,
    TemplateType, // e.g., [Template] @Var
    CustomType,   // e.g., [Custom] @Element
};

struct Constraint {
    ConstraintType type;
    std::string identifier; // e.g., "span", "Var", "Element"
};

struct ElementNode : public BaseNode {
    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
    std::vector<Constraint> constraints;
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

    void print(int level = 0) const override {
        for (int i = 0; i < level; ++i) std::cout << "  ";
        std::cout << "ElementNode (" << tagName << ")" << std::endl;
        for (const auto& attr : attributes) {
            attr->print(level + 1);
        }
        for (const auto& child : children) {
            child->print(level + 1);
        }
    }
};
