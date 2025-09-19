#ifndef CHTL_VISITOR_H
#define CHTL_VISITOR_H

// Forward declarations of the node types to avoid circular dependencies
// with the node headers.
class ElementNode;
class TextNode;
class CommentNode;

// Defines the interface for a visitor that can traverse the AST.
// This allows for separating the AST structure from the operations performed on it.
class Visitor {
public:
    virtual ~Visitor() = default;

    // A visit method for each concrete node type in the AST.
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
};

#endif // CHTL_VISITOR_H
