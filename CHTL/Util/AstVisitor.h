#ifndef CHTL_AST_VISITOR_H
#define CHTL_AST_VISITOR_H

// Forward declarations for the node types are necessary here
// to avoid circular dependencies with the Node header files.
class ElementNode;
class TextNode;
class CommentNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
};

#endif // CHTL_AST_VISITOR_H
