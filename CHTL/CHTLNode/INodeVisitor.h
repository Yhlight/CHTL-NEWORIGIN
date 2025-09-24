#ifndef CHTL_INODE_VISITOR_H
#define CHTL_INODE_VISITOR_H

namespace CHTL
{
    namespace AST
    {
        // Forward declarations for all concrete node types
        class ElementNode;
        class AttributeNode;
        class TextNode;
        class StyleNode;
        class CommentNode;
        // Add more as they are created...

        class INodeVisitor
        {
        public:
            virtual ~INodeVisitor() = default;

            virtual void Visit(ElementNode& node) = 0;
            virtual void Visit(AttributeNode& node) = 0;
            virtual void Visit(TextNode& node) = 0;
            virtual void Visit(StyleNode& node) = 0;
            virtual void Visit(CommentNode& node) = 0;
            // Add more Visit methods as new node types are created...
        };
    }
}

#endif // CHTL_INODE_VISITOR_H
