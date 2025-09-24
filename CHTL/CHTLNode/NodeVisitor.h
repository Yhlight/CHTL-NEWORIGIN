#pragma once

namespace CHTL {

// Forward declare all node types that the visitor can visit.
// This prevents circular dependencies between node headers and the visitor header.
class BaseNode;
class ElementNode;
class TextNode;
// ... other node types will be added here later.

class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    // The visit methods for each concrete node type.
    // The implementation of these in a concrete visitor will define the operation.
    virtual void visit(BaseNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    // virtual void visit(TextNode& node) = 0;
};

} // namespace CHTL
