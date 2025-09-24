#pragma once

#include <memory>
#include <vector>

// Enum to identify the type of an AST node.
enum class NodeType {
    Element,
    Text,
    Comment,

    // Template Definitions
    StyleTemplate,
    ElementTemplate,
    VarTemplate,

    // Utility Nodes
    Fragment,

    // Raw Content
    Origin,

    // Statements
    Import
    // More types will be added as more CHTL features are implemented.
};

// Base class for all nodes in the Abstract Syntax Tree (AST).
// It's an abstract class that defines the common interface for all nodes.
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // Returns the type of the node. This is useful for type-safe downcasting.
    virtual NodeType getType() const = 0;
};
