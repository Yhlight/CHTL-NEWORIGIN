#pragma once
#include <string>
#include <vector>
#include <memory>

namespace chtl::ast {

enum class NodeKind {
    Base,
    Element,
    Text,
    Comment,
    Template,
    Custom,
    Style,
    Script,
    Origin,
    Import,
    Config,
    Namespace,
    Operator
};

class Node {
public:
    explicit Node(NodeKind k) : kind_(k) {}
    virtual ~Node() = default;

    NodeKind kind() const { return kind_; }
    void addChild(std::shared_ptr<Node> child) { children_.push_back(std::move(child)); }
    const std::vector<std::shared_ptr<Node>>& children() const { return children_; }

private:
    NodeKind kind_ { NodeKind::Base };
    std::vector<std::shared_ptr<Node>> children_;
};

} // namespace chtl::ast

