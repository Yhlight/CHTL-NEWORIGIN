#pragma once

#include <memory>

namespace CHTL {

// Forward declarations for all concrete node types.
class ElementNode;
class TextNode;
class CommentNode;
// ... other node types will be added here.

// The NodeVisitor interface, part of the Visitor design pattern.
// This allows us to perform operations on the AST without coupling the operations
// to the node classes themselves. The generator will be a primary implementation of this.
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visit(const ElementNode& node) = 0;
    virtual void visit(const TextNode& node) = 0;
    virtual void visit(const CommentNode& node) = 0;
    // ... visit methods for other node types will be added here.
};

// The abstract base class for all nodes in the AST.
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // The 'accept' method is the entry point for the Visitor pattern.
    virtual void accept(NodeVisitor& visitor) const = 0;
};

} // namespace CHTL
