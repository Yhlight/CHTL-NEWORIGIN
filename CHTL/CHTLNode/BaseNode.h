#pragma once

#include <vector>
#include <string>
#include <memory>

// Forward declarations for the Visitor pattern
class NodeVisitor;
class ElementNode;
class TextNode;
class CommentNode;

// Base class for all AST nodes
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(NodeVisitor& visitor) = 0;
    virtual std::shared_ptr<BaseNode> clone() const = 0;
};

// Visitor interface - enables clean processing of the AST
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
};
