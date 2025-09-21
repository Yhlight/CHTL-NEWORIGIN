#pragma once

#include <memory>

namespace CHTL {

// Forward declare the Visitor class
class Visitor;

// A base class for all nodes in the Abstract Syntax Tree (AST).
// It's a common practice to have a base class for polymorphism,
// for example, when storing children of different node types in a collection
// or for implementing the Visitor design pattern for tree traversal.
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // The accept method is the entry point for the visitor.
    virtual void accept(Visitor& visitor) = 0;
};

} // namespace CHTL
