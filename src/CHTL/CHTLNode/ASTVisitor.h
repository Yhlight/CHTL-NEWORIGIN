#pragma once

// Forward declarations of all node types
namespace CHTL {
    class ElementNode;
    class TextNode;
    class CommentNode;
    class AttributeNode;
}

namespace CHTL {

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
};

} // namespace CHTL
