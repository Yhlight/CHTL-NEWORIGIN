#pragma once

#include <memory>
#include <vector>
#include <string>

// Enum to identify the type of an AST node.
enum class NodeType {
    Element,
    Text,
    Comment,
    Fragment,
    Origin,
    Script,
    Conditional,

    // Top-level directives
    Namespace,
    Config,
    Info,
    Import,
    Use,

    // Template and Customization Definitions
    StyleTemplate,
    ElementTemplate,
    VarTemplate,
    Custom, // Placeholder for [Custom] blocks

    // CHTL JS Specific
    EnhancedSelector,
    RawScript,
    ResponsiveValue,

    // Misc
    Constraint
};

// Base class for all nodes in the Abstract Syntax Tree (AST).
// It's an abstract class that defines the common interface for all nodes.
class BaseNode {
public:
    virtual ~BaseNode() = default;

    // Returns the type of the node. This is useful for type-safe downcasting.
    virtual NodeType getType() const = 0;

    // Optional: Tracks the name of the template this node originated from.
    std::string sourceTemplateName;
};
