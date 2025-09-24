#ifndef CHTL_AST_VISITOR_H
#define CHTL_AST_VISITOR_H

namespace CHTL {

// Forward declare all concrete node types that the visitor can visit.
class ElementNode;
class TextNode;
class CommentNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;

    // A visit method for each concrete node type
    virtual void Visit(ElementNode& node) = 0;
    virtual void Visit(TextNode& node) = 0;
    virtual void Visit(CommentNode& node) = 0;
};

} // namespace CHTL

#endif // CHTL_AST_VISITOR_H
