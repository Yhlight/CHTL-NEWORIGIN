#ifndef CHTL_NODE_VISITOR_H
#define CHTL_NODE_VISITOR_H

namespace CHTL {

// Forward declarations of all node types
class ProgramNode;
class ElementNode;
class TextNode;
class CommentNode;
class AttributeNode;
class StyleNode;
class ScriptNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ImportNode;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
};

} // namespace CHTL

#endif // CHTL_NODE_VISITOR_H
