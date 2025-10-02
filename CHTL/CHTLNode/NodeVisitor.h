#ifndef CHTL_NODE_VISITOR_H
#define CHTL_NODE_VISITOR_H

// Forward declarations of all node types
class DocumentNode;
class ElementNode;
class TextNode;
class CommentNode;
class StyleNode;
class CssRuleNode;
class TemplateStyleNode;
class TemplateElementNode;
class TemplateVarNode;
class CustomStyleNode;
class CustomElementNode;
class CustomVarNode;
class ImportNode;
class NamespaceNode;
class UseNode;
class DeclarationNode;

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    virtual void visit(DocumentNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(CssRuleNode& node) = 0;
    virtual void visit(TemplateStyleNode& node) = 0;
    virtual void visit(TemplateElementNode& node) = 0;
    virtual void visit(TemplateVarNode& node) = 0;
    virtual void visit(CustomStyleNode& node) = 0;
    virtual void visit(CustomElementNode& node) = 0;
    virtual void visit(CustomVarNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(UseNode& node) = 0;
    virtual void visit(DeclarationNode& node) = 0;
};

#endif // CHTL_NODE_VISITOR_H