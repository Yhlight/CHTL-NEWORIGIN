#pragma once

#include <memory>
#include <vector>

enum class NodeType {
    Root,
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
    TemplateDefinition,
    TemplateUsage,
    Import
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const { return NodeType::Base; }
    virtual std::unique_ptr<BaseNode> clone() const = 0;

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const {
        return children;
    }

    std::vector<std::unique_ptr<BaseNode>>& getChildren() {
        return children;
    }

private:
    std::vector<std::unique_ptr<BaseNode>> children;
};