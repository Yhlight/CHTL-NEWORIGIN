#pragma once

#include <memory>
#include <vector>

enum class NodeType {
    Base,
    Element,
    Text,
    Comment,
    Style,
    Script,
    StyleProperty,
    ClassSelector,
    IdSelector,
    ContextSelector,
    Reference,
    Value,
    BinaryOp,
    Conditional,
    IfBehavior,
    ElseIfBehavior,
    ElseBehavior,
    TemplateDefinition,
    TemplateUsage,
    CustomDefinition,
    CustomUsage
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const { return NodeType::Base; }
    virtual std::unique_ptr<BaseNode> clone() const {
        auto node = std::make_unique<BaseNode>();
        for (const auto& child : children) {
            node->addChild(child->clone());
        }
        return node;
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

    std::vector<std::unique_ptr<BaseNode>>& getMutableChildren() {
        return children;
    }

private:
    std::vector<std::unique_ptr<BaseNode>> children;
};