#pragma once

// Forward declarations of the node classes to be visited.
// This avoids circular header dependencies between the nodes and the visitor.
namespace CHTL {
class ProgramNode;
class ElementNode;
class TextNode;

// Defines the interface for a visitor that can traverse the AST.
// Each concrete node type has a corresponding `visit` method.
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
};
} // namespace CHTL
