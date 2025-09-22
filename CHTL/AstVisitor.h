#pragma once

// Forward declare the node types to avoid circular dependencies
class ElementNode;
class TextNode;
class CommentNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(const ElementNode& node) = 0;
    virtual void visit(const TextNode& node) = 0;
    virtual void visit(const CommentNode& node) = 0;
};
