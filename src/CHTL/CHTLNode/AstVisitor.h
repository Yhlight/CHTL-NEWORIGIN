#pragma once

namespace CHTL
{
    // Forward-declare all concrete AST node classes.
    // This avoids circular dependencies, as the node classes
    // will need to include this file.
    class ElementNode;
    class TextNode;
    class CommentNode;
    class StyleNode;
    class LiteralExprNode;
    class BinaryOpExprNode;
    class PropertyAccessExprNode;
    class GroupExprNode;

    // The AstVisitor interface declares a set of visiting methods that
    // can be used to traverse the AST.
    class AstVisitor
    {
    public:
        virtual ~AstVisitor() = default;

        virtual void visit(ElementNode &node) = 0;
        virtual void visit(TextNode &node) = 0;
        virtual void visit(CommentNode &node) = 0;
        virtual void visit(StyleNode &node) = 0;
        virtual void visit(LiteralExprNode &node) = 0;
        virtual void visit(BinaryOpExprNode &node) = 0;
        virtual void visit(PropertyAccessExprNode &node) = 0;
        virtual void visit(GroupExprNode &node) = 0;
    };
}
