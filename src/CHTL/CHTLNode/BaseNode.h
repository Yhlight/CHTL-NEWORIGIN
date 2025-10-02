#pragma once

#include <vector>
#include <memory>

// Forward declare the visitor
class NodeVisitor;

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual void accept(NodeVisitor& visitor) = 0;
};

// Forward declare node types
class ElementNode;
class TextNode;
class CommentNode;
class AttributeNode;

// Define a visitor interface for the AST
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
};