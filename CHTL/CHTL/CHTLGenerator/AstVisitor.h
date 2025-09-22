#pragma once

namespace CHTL {

// Forward declarations of node types
class ElementNode;
class TextNode;
class AttributeNode;
class StyleBlockNode;
class CssPropertyNode;

class AstVisitor {
public:
    virtual ~AstVisitor() = default;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
    virtual void visit(CssPropertyNode& node) = 0;
};

} // namespace CHTL
